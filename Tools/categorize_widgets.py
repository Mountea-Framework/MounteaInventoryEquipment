"""Categorize UMG Designer templates and manually declared widget references.

From the plugin root (Python 3, with Unreal Engine installed):
    python Tools/categorize_widgets.py /MounteaAdvancedInventorySystem/WBP/V4
    python Tools/categorize_widgets.py Content/WBP/V4 --apply

In the Unreal Editor Output Log, using its Cmd input:
    py "D:/path/to/Tools/categorize_widgets.py" /Game/UI --apply

The path is recursive. Preview is the default; --apply compiles and saves each
changed Blueprint. From a shell the tool reads saved packages in a separate
editor process; use the Output Log with open/unsaved assets. A category is
assigned even to Designer controls whose Is Variable checkbox is off, for use
if they are exposed later. The checkbox itself is preserved.
"""
import argparse
import json
from pathlib import Path
import re
import subprocess
import sys
import tempfile

try:
    import unreal
except ImportError:
    unreal = None


def package_folder(value):
    folder = Path(value)
    if folder.is_dir():
        folder = folder.resolve()
        for parent in (folder, *folder.parents):
            if parent.name.lower() != "content":
                continue
            plugins = list(parent.parent.glob("*.uplugin"))
            if len(plugins) == 1:
                mount = plugins[0].stem
            elif list(parent.parent.glob("*.uproject")):
                mount = "Game"
            else:
                continue
            return "/".join(("", mount, *folder.relative_to(parent).parts))
    value = value.rstrip("/")
    if not value.startswith("/") or any(part in ("", ".", "..") for part in value[1:].split("/")):
        raise ValueError("Specify a Content Browser folder or an existing directory under Content.")
    return value


def category_for_class(cls):
    widget = unreal.get_default_object(cls)
    if not isinstance(widget, unreal.Widget):
        return None
    for base in type(widget).__mro__:
        name = base.__name__
        if name in ("EditableText", "EditableTextBox", "MultiLineEditableText",
                    "MultiLineEditableTextBox", "TextBlock", "RichTextBlock"):
            return "Widgets|Text"
        if name in ("CommonButtonBase", "MounteaAdvancedInventoryButtonWidget"):
            return "Widgets|Button"
        if hasattr(base, "static_class") and base.static_class().get_path_name().startswith("/Script/UMG."):
            return "Widgets|" + name
    return "Widgets|" + cls.get_name().removesuffix("_C")


def export_blueprint(bp):
    with tempfile.TemporaryDirectory(prefix="widget_categories_") as directory:
        path = Path(directory) / "widget.t3d"
        task = unreal.AssetExportTask()
        task.object = bp
        task.exporter = unreal.ObjectExporterT3D()
        task.filename = str(path)
        task.automated = True
        task.prompt = False
        if not unreal.Exporter.run_asset_export_task(task):
            raise RuntimeError("Export failed for " + bp.get_path_name())
        data = path.read_bytes()
        return data.decode("utf-16" if data.startswith((b"\xff\xfe", b"\xfe\xff")) else "utf-8-sig")


def designer_widgets(bp, text):
    tree = unreal.find_object(bp, "WidgetTree")
    if tree is None:
        return {}
    names = set()
    categories = {}
    stack = []
    for line in text.splitlines():
        line = line.strip()
        if line.startswith("Begin Object"):
            stack.append(re.search(r'\bName="([^"]+)"', line)[1])
            if len(stack) == 3 and stack[1] == "WidgetTree":
                names.add(stack[-1])
        elif line == "End Object":
            stack.pop()
        elif stack[1:] == ["WidgetTree"] and line.startswith("AllWidgets("):
            names.add(line.rsplit("'", 2)[1].rsplit(".", 1)[-1])
        elif len(stack) == 3 and stack[1] == "WidgetTree" and line.startswith("CategoryName="):
            categories[stack[-1]] = line.partition("=")[2].strip('"')
    result = {}
    for name in sorted(names):
        widget = unreal.find_object(tree, name)
        if widget is None:
            raise RuntimeError(f"Cannot resolve {bp.get_name()}.{name}")
        if isinstance(widget, unreal.Widget):
            result[name] = (widget, categories.get(name, ""))
    return result


def set_property(obj, prop, serialized):
    path = obj.get_path_name()
    # UE's SETNOPEC parser stores the object path in a 256-TCHAR buffer.
    if len(path.encode("utf-16-le")) // 2 >= 256 or not path.startswith("/") or '"' in path:
        raise ValueError("Object path cannot be addressed exactly: " + path)
    world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
    unreal.SystemLibrary.execute_console_command(world, f'SETNOPEC "{path}" {prop} {serialized}')


def _manual_fields(text):
    """Split Unreal struct text without splitting quoted text or nested structs."""
    text = text.strip()
    if not (text.startswith("(") and text.endswith(")")):
        raise ValueError("Expected a parenthesized Unreal struct")
    fields = []
    start = 1
    depth = 0
    quoted = False
    escaped = False
    for index, character in enumerate(text[1:-1], 1):
        if quoted:
            if escaped:
                escaped = False
            elif character == "\\":
                escaped = True
            elif character == '"':
                quoted = False
        elif character == '"':
            quoted = True
        elif character == "(":
            depth += 1
        elif character == ")":
            depth -= 1
        elif character == "," and depth == 0:
            fields.append(text[start:index])
            start = index + 1
        if depth < 0:
            raise ValueError("Unbalanced Unreal struct")
    if depth or quoted:
        raise ValueError("Unterminated Unreal struct")
    if text[start:-1].strip():
        fields.append(text[start:-1])
    return fields


def _manual_replace_category(original, category_text):
    fields = _manual_fields(original)
    indices = [index for index, field in enumerate(fields)
               if field.split("=", 1)[0].strip() == "Category"]
    if len(indices) > 1:
        raise ValueError("Duplicate Category field")
    if indices:
        fields[indices[0]] = category_text
    else:
        fields.append(category_text)
    return "(" + ",".join(fields) + ")"


def _manual_variable_rows(bp, t3d):
    """Only read this Blueprint's own NewVariables, excluding nested objects."""
    rows = []
    depth = 0
    root_seen = False
    for line in t3d.splitlines():
        stripped = line.strip()
        if stripped.startswith("Begin Object"):
            if depth == 0:
                if root_seen:
                    raise ValueError("Expected one exported Blueprint object")
                root_seen = True
                if bp.get_path_name() not in stripped:
                    raise ValueError("T3D does not identify the requested Blueprint")
            depth += 1
        elif stripped == "End Object":
            depth -= 1
        elif depth == 1:
            match = re.fullmatch(r"NewVariables\((\d+)\)=(.*)", stripped)
            if match:
                index = int(match.group(1))
                if index != len(rows):
                    raise ValueError("Noncontiguous or duplicated NewVariables indices")
                rows.append(match.group(2))
    if depth or not root_seen:
        raise ValueError("Incomplete Blueprint T3D export")
    return rows



def fields(text):
    return dict(part.split("=", 1) for part in _manual_fields(text))


def reference_category(pin, prefix):
    if pin.get(prefix + "Category", "").strip('"') not in ("object", "softobject"):
        return None
    reference = pin.get(prefix + "SubCategoryObject", "")
    match = re.search(r"'([^']+)'", reference)
    if match:
        cls = unreal.load_object(None, match[1])
        if not isinstance(cls, unreal.Class):
            raise ValueError("Unable to resolve declared reference class " + reference)
        return category_for_class(cls)
    return None


def manual_plan(bp, text):
    rows = _manual_variable_rows(bp, text)
    desired = list(rows)
    changes = []
    for index, row in enumerate(rows):
        desc = fields(row)
        pin = fields(desc.get("VarType", "()"))
        category = reference_category(pin, "Pin")
        if not category and "PinValueType" in pin:
            category = reference_category(fields(pin["PinValueType"]), "Terminal")
        if not category:
            continue
        old = desc.get("Category", "")
        if old in (f'INVTEXT("{category}")', f'"{category}"'):
            continue
        desired[index] = _manual_replace_category(row, f'Category=INVTEXT("{category}")')
        changes.append((desc["VarName"].strip('"'), old, category))
    return rows, desired, changes


def verify_members(bp, text, expected):
    actual = _manual_variable_rows(bp, text)
    # Property order can change when an omitted Category is inserted.
    if [fields(v) for v in actual] != [fields(v) for v in expected]:
        raise RuntimeError("Member variable metadata verification failed: " + bp.get_path_name())


def organize(bp, apply):
    text = export_blueprint(bp)
    widgets = designer_widgets(bp, text)
    planned = []
    for name, (widget, old) in widgets.items():
        category = category_for_class(widget.get_class())
        if category != old:
            planned.append((name, old, category))
    before, desired, members = manual_plan(bp, text)
    for kind, changes in (("Designer", planned), ("Member", members)):
        for name, old, new in changes:
            print(f"{bp.get_path_name()}  {kind} {name}: {old or '(default)'} -> {new}")
    if not (apply and (planned or members)):
        return len(planned) + len(members)

    def verify(snapshot):
        captured = designer_widgets(bp, snapshot)
        expected = {n: old for n, (_, old) in widgets.items()}
        expected.update({n: category for n, _, category in planned})
        if {n: old for n, (_, old) in captured.items()} != expected:
            raise RuntimeError("Designer category verification failed: " + bp.get_path_name())
        verify_members(bp, snapshot, desired)

    try:
        bp.modify()
        for name, _, category in planned:
            widget = widgets[name][0]
            widget.modify()
            set_property(widget, "CategoryName", category)
        if members:
            set_property(bp, "NewVariables", "(" + ",".join(desired) + ")")
        verify(export_blueprint(bp))
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        compiled = export_blueprint(bp)
        if re.search(r"^   Status=BS_Error\s*$", compiled, re.M):
            raise RuntimeError("Compile failed: " + bp.get_path_name())
        verify(compiled)
        if not unreal.EditorAssetLibrary.save_loaded_asset(bp, only_if_is_dirty=False):
            raise RuntimeError("Could not save " + bp.get_path_name())
    except Exception:
        current = designer_widgets(bp, export_blueprint(bp))
        for name, old, _ in planned:
            set_property(current[name][0], "CategoryName", json.dumps(old, ensure_ascii=False))
        if members:
            set_property(bp, "NewVariables", "(" + ",".join(before) + ")")
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        raise
    return len(planned) + len(members)


def run(folder, apply=False):
    registry = unreal.AssetRegistryHelpers.get_asset_registry()
    registry.scan_paths_synchronous([folder], force_rescan=False)
    if not unreal.EditorAssetLibrary.does_directory_exist(folder):
        raise ValueError("Content Browser folder does not exist: " + folder)
    query = unreal.ARFilter(package_paths=[folder], recursive_paths=True, recursive_classes=True,
                            class_paths=[unreal.TopLevelAssetPath("/Script/UMGEditor", "WidgetBlueprint")])
    assets = sorted(registry.get_assets(query), key=lambda a: str(a.package_name))
    changed = assignments = 0
    for asset in assets:
        bp = asset.get_asset()
        if not isinstance(bp, unreal.WidgetBlueprint):
            raise RuntimeError("Unable to load " + str(asset.package_name))
        count = organize(bp, apply)
        assignments += count
        changed += bool(count)
    print(f"{'Applied' if apply else 'Preview'}: {assignments} assignments in {changed} of {len(assets)} Widget Blueprints.")


def launch(project, executable, folder, apply):
    if not executable:
        if sys.platform != "win32":
            raise ValueError("Supply --editor with your UnrealEditor-Cmd executable.")
        import winreg
        association = json.loads(project.read_text(encoding="utf-8-sig"))["EngineAssociation"]
        for hive, subkey, value in (
            (winreg.HKEY_LOCAL_MACHINE, rf"SOFTWARE\EpicGames\Unreal Engine\{association}", "InstalledDirectory"),
            (winreg.HKEY_CURRENT_USER, r"SOFTWARE\Epic Games\Unreal Engine\Builds", association),
        ):
            try:
                with winreg.OpenKey(hive, subkey) as handle:
                    executable = Path(winreg.QueryValueEx(handle, value)[0]) / "Engine/Binaries/Win64/UnrealEditor-Cmd.exe"
                break
            except OSError:
                continue
        if not executable:
            raise ValueError("Engine installation not found. Supply --editor.")
    with tempfile.TemporaryDirectory(prefix="categorize_widgets_") as directory:
        script = Path(directory) / "run.py"
        success = Path(directory) / "complete"
        script.write_text("import runpy\nfrom pathlib import Path\n"
                          f"runpy.run_path({str(Path(__file__).resolve())!r})['run']({folder!r}, {apply!r})\n"
                          f"Path({str(success)!r}).touch()\n", encoding="utf-8")
        args = [str(executable), str(project.resolve()), "-run=pythonscript", "-unattended", "-nullrhi",
                "-nop4", "-stdout", "-FullStdOutLogOutput", "-DisablePlugins=MounteaDialogueSystem,MounteaInteractionSystem"]
        command = (subprocess.list2cmdline(args) + f' -script="{script.as_posix()}"'
                   if sys.platform == "win32" else args + [f"-script={script}"])
        result = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True, errors="replace")
        for line in result.stdout.splitlines():
            if ("LogPython:" in line and "Warning:" not in line) or "Error:" in line:
                print(line.partition("LogPython: ")[2] or line)
        if result.returncode or not success.exists():
            raise SystemExit("Widget categorization failed.")


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("folder", help="Content Browser path or content directory")
    parser.add_argument("--apply", action="store_true", help="Save the planned assignments")
    parser.add_argument("--project", type=Path, help="Host .uproject for command-line runs")
    parser.add_argument("--editor", type=Path, help="Override UnrealEditor-Cmd executable")
    args = parser.parse_args()
    folder = package_folder(args.folder)
    if unreal is not None:
        run(folder, args.apply)
        return
    project = args.project
    if project is None:
        project = next((p for parent in Path(__file__).resolve().parents for p in parent.glob("*.uproject")), None)
    if project is None:
        parser.error("Supply --project with the path to the host .uproject")
    launch(project, args.editor, folder, args.apply)


if __name__ == "__main__":
    main()
