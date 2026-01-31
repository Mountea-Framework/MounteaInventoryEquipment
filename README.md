# Mountea Inventory & Equipment

Mountea Inventory & Equipment is an Open-source Mountea Framework Unreal Engine tool for easy inventory management with flexible interactions.

[![UE](https://img.shields.io/badge/supported-5.6%2B-green)](https://github.com/Mountea-Framework/ActorInventoryPlugin/master/README.md#compatible-engine-version)
[![License](https://img.shields.io/github/license/Mountea-Framework/MounteaInventoryEquipment?style=flat-square)](https://github.com/Mountea-Framework/MounteaInventoryEquipment/blob/main/LICENSE)
[![YouTube](https://img.shields.io/badge/YouTube-Subscribe-red?style=flat&logo=youtube)](https://www.youtube.com/@mounteaframework)
[![Discord](https://badgen.net/discord/online-members/2vXWEEN?label=Discord&logo=discord&logoColor=ffffff&color=7389D8)](https://discord.com/invite/2vXWEEN)
[![Discord](https://badgen.net/discord/members/2vXWEEN?label=Discord&logo=discord&logoColor=ffffff&color=7389D8)](https://discord.com/invite/2vXWEEN)


## Discord Channel

> [!TIP]
>
> Join the [Discord Server](https://discord.gg/waYT2cn37z) where you can raise tickets, ask questions, get help or just share your progress.
>
> <a href="https://discord.gg/waYT2cn37z"><img src="https://anbhelp.zendesk.com/hc/article_attachments/360021176052/discord.gif" alt="Discord LINK" width="100"></a>


## Documentation

> [!IMPORTANT]
>
> We have a brand new documentation page! Feel free to take a look and learn how to install, setup, customize and use the system in your own projects.
>
> <a href="https://mountea.tools/docs/advancedinventoryequipmentsystem/Overview/"> <p align="center" width="100%"><img width="90%" src="https://github.com/user-attachments/assets/c1970005-fe66-4cd0-9ce7-32ed03640cec"></p></a>


## Tutorial videos

> [!IMPORTANT]
>
> Visit our [YouTube Channel](https://www.youtube.com/playlist?list=PLIU53wA8zZmg5eBKEcpZr7G8JBBZ4QPKq) where we post educational videos.
>
> <a href="https://www.youtube.com/playlist?list=PLIU53wA8zZmg5eBKEcpZr7G8JBBZ4QPKq"><img src="https://cdnl.iconscout.com/lottie/premium/thumb/youtube-8622218-6842833.gif" alt="TUTORIAL playlist" width="150"></a>


## Compatible Engine Version

> [!CAUTION]
>
> **Legend**
> Colour | Explanation
> -------------- | --------------
> 🟩 | Supported
> 🟨 | WIP
> 🟪 | Deprecated (abandoned)
> 🟥 | Partially supported ([Epic Games Policies](https://www.unrealengine.com/en-US/marketplace-guidelines#263d))
>
> **Support Table**
> Engine Version | Plugin Version | Support
> -------------- | -------------- | ----
> UE 5.6 | - | 🟨


## Features

### Item Templates Editor

> Allows in-editor bulk changes with options to export to `mnteaitem` or `mnteaitems` files for edits outside of Unreal Engine.
>
> <p align="center" width="100%">
>     <img width="45%" src="https://github.com/user-attachments/assets/557a0676-b974-4926-ab2b-5f85ca9788f2">
>     <img width="45%" src="https://github.com/user-attachments/assets/fa76e02d-3fed-4367-a5c5-786a1f4ba4da">
> </p>

### Data-Driven Approach

> The whole system is driven by configuration, which is designed to natually follow order of things:
>
> - Project Settings to define global configuration files
> - - Inventory Configuration to define gameplay variables
> - - Inventory UI Configuration to define User Interface settings
> - Etc.
> <p align="center" width="100%">
>     <img width="30%" src="https://github.com/user-attachments/assets/bd88891d-d1d9-4bce-b74f-2a52b05d2809">
>     <img width="30%" src="https://github.com/user-attachments/assets/05ddd769-9784-4974-ad1e-75ea7522b7f3">
>     <img width="30%" src="https://github.com/user-attachments/assets/59889a6c-1e04-4916-ac44-846bf7e22ef4">
> </p>

### Custom K2Nodes

> Custom K2Nodes are designed to provide maximum efficiency for your development speed.
>
> Using GameplayTags for Input for UI mapping is one example of how the development can be simplified. Rather than searching all tags in the system, only the `Input` ones are allowed.
>
> For processing UI Command we also have a custom Node, which allows custom inputs, as well as selection from pre-defined commands!
>
> <p align="center" width="100%">
>    <img width="30%" src="https://github.com/user-attachments/assets/739c356a-bd21-4c77-b2d3-cfc5efaaee76">
>    <img width="30%" src="https://github.com/user-attachments/assets/9b1a13b0-9ad4-4802-876a-c7a42f367ed6">
>    <img width="30%" src="https://github.com/user-attachments/assets/0a2f66c1-92a6-4c7c-8e70-9a50d414f621">
> </p>

### Custom UI Driven Input

> The Inventory & Equipment come with a custom UI driven Input System based on a custom mapping sheet, which supports dynamic runtime remapping and lazy icon loading.
>
> The system is built atop Enhanced Input; however, it is mainly focused on UI, with custom states filtering, priorities and hardware mapping.
>
> <p align="center" width="100%">
>     <img width="45%" src="https://github.com/user-attachments/assets/3d543fa0-da4e-463b-a4ca-53037e152fac">
>     <img width="45%" src="https://github.com/user-attachments/assets/727405e8-d62a-4982-9ada-bdb3830c7e1f">
> </p>

### UI Actions

> UI Actions are a custom solution built to provide as approachable as possible toolset to create Item-based actions, such as, yet not limited to, Consume, Drop, Favourite, Equip and more.

### Equipment Sockets

> Equipment Sockets allow a user-defined way to specify what part of any Character can be equipped with what Inventory Item.
> A few, the most common sockets are pre-defined and shipped with the plugin.
> 
> <p align="center" width="100%">
>     <img width="90%" src="https://github.com/user-attachments/assets/666c8401-ed4e-4cf5-82f4-a02ae881188c">
> </p>
---

## Star History

> <a href="https://www.star-history.com/#Mountea-Framework/MounteaInventoryEquipment&type=date&legend=top-left">
>  <picture>
>    <source media="(prefers-color-scheme: dark)" srcset="https://api.star-history.com/svg?repos=Mountea-Framework/MounteaInventoryEquipment&type=date&theme=dark&legend=top-left" />
>    <source media="(prefers-color-scheme: light)" srcset="https://api.star-history.com/svg?repos=Mountea-Framework/MounteaInventoryEquipment&type=date&legend=top-left" />
>    <img alt="Star History Chart" src="https://api.star-history.com/svg?repos=Mountea-Framework/MounteaInventoryEquipment&type=date&legend=top-left" />
>  </picture>
> </a>
