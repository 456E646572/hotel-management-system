# 项目说明

本项目为大一高级语言程序设计大作业。

关于本项目的设计，详见 `9_酒店管理系统.pdf`。

本项目用到了很多 c++17 的特性，请确保使用的编译器版本够高并使用 `-std=c++17` 编译。

如果您是旧版 devc++ 受害者，您可以在[这里](https://github.com/niXman/mingw-builds-binaries/releases)在找到最新版本的编译器，并跟着[这篇教程](https://zhuanlan.zhihu.com/p/438701242)配置devc++。

本项目用到了：

- 各种 c++ 基础语法
- `template<typename T>` 的使用
- `<string>` 相关操作
- `<memory>` 中的智能指针
- 结构体、单链表
- `<fstream>` 文件读写
- [ctime库的使用](https://blog.csdn.net/u013043408/article/details/83615582)，以及 `<sstream>` 和 `<iomanip>` 用于读入指定格式的时间
- 用 `<stdexcept>` 中的 `try-catch` 处理输入信息时的报错
- mermeid 绘制示意图


# 功能模块示意图

```mermaid
flowchart TD
    A[菜单，选择操作文件]
    A-->|命令 1|GUEST[客人]-->B1[选择具体操作]
    A-->|命令 2|ROOM[客房]-->B2[选择具体操作]
    A-->|命令 3|SYS_OUT[退出系统]-->SAVE[写入文件]

    B1-->|命令 1|GUEST_QUERY[
        查询客人信息
        1.全部客人信息
        2.指定客人信息
    ]

    B1-->|命令 2|GUEST_ADD[新增客人信息]

    B1-->|命令 3|GUEST_DEL[用身份证号删除指定客人]

    B1-->|命令 4|GUEST_MOD[
        用身份证号修改客人信息
        1.姓名
        2.是否离开
        3.入住时间
        4.离店时间
        5.住宿房间
    ]

    B2-->|命令 1|ROOM_QUERY[
        查询客房信息:
        1.全部房间信息
        2.指定房间信息
        3.指定价格区间
    ]

    B2-->|命令 2|ROOM_ADD[新增客房信息]

    B2-->|命令 3|ROOM_DEL[用房间号删除指定客房]

    B2-->|命令 4|ROOM_MOD[
        用房间号修改客房信息
        1.类型
        2.价格
    ]

    END_TURN[一轮操作结束]
    GUEST_QUERY-->END_TURN
    GUEST_ADD-->END_TURN
    GUEST_DEL-->END_TURN
    GUEST_MOD-->END_TURN
    ROOM_QUERY-->END_TURN
    ROOM_ADD-->END_TURN
    ROOM_DEL-->END_TURN
    ROOM_MOD-->END_TURN  

    END_TURN-->NEICUN[修改写入内存]-->A
    SAVE[写入文件]-->END[退出系统]
```

# 数据结构示意图

```mermaid
classDiagram
class Guest{
    +string ID
    +string name
    +time_t checkin_time
    +time_t checkout_time
    +double cost
    +RoomList room

    +operator==()
    +add_attribute()
    +calc_cost()
    +print_info()
    +print_info(fstream)
}

class Room{
    +string ID
    +string type
    +double price
    +bool isOccupied

    +operator==()
    +add_attribute()
    +print_info()
    +print_info(fstream)
}

class list~T~{
    +T data
    +Node* nxt
}

Guest .. list
Room  .. list
```

# 函数调用示意图

```mermaid
flowchart TD
    subgraph init
        READ1["`read<Room>(ROOMFILE, rl)`"]~~~READ2["`read<Guest>(GUESTFILE, gl)`"]~~~SET[更新房间占用情况]
    end

    subgraph END
        WRITE1["`write<Room>(ROOMFILE, rl)`"]~~~WRITE2["`write<Guest>(GUESTFILE, gl)`"]
    end

    subgraph data_GUEST
        GUEST_QUERY["`find&lt;Guest&gt;()`"]
        GUEST_ADD["`add&lt;Guest&gt;()`"]
        GUEST_DEL["`del&lt;Guest&gt;()`"]
        GUEST_MOD["`Guest.calc_cost()`"]
        GUEST_INFO["`Guest.print_info()`"]
        GUEST_CALC["`Guest.calc_cost()`"]
    end

    subgraph data_ROOM
        ROOM_QUERY["`find&lt;Room&gt;()`"]
        ROOM_ADD["`add&lt;Room&gt;()`"]
        ROOM_DEL["`del&lt;Room&gt;()`"]
        ROOM_QUERY["`find_room()`"]
        ROOM_INFO["`Room.print_info()`"]
    end

    subgraph data
        data_GUEST
        data_ROOM
    end

    subgraph OP_GUEST
        COMMAND1["`command()`"]
        COMMAND1-->OP_GUEST_QUERY["`query_guest()`"]
        COMMAND1-->OP_GUEST_ADD["`add_guest()`"]
        COMMAND1-->OP_GUEST_DEL["`del_guest()`"]
        COMMAND1-->OP_GUEST_MOD["`modify_guest()`"]

        OP_GUEST_QUERY-->GUEST_QUERY
        OP_GUEST_QUERY-->GUEST_INFO
        
        OP_GUEST_ADD-->GUEST_QUERY
        OP_GUEST_ADD-->GUEST_ADD
        OP_GUEST_ADD-->GUEST_INFO

        OP_GUEST_DEL-->GUEST_QUERY
        OP_GUEST_DEL-->GUEST_DEL

        OP_GUEST_MOD-->GUEST_QUERY
        OP_GUEST_MOD-->ROOM_QUERY
        OP_GUEST_MOD-->GUEST_CALC
        OP_GUEST_MOD-->GUEST_INFO
    end

    subgraph OP_ROOM
        COMMAND2["`command()`"]
        COMMAND2-->OP_ROOM_QUERY["`query_room()`"]
        COMMAND2-->OP_ROOM_ADD["`add_room()`"]
        COMMAND2-->OP_ROOM_DEL["`del_room()`"]
        COMMAND2-->OP_ROOM_MOD["`mod_room()`"]

        OP_ROOM_QUERY-->ROOM_QUERY
        OP_ROOM_QUERY-->ROOM_INFO
        
        OP_ROOM_ADD-->ROOM_QUERY
        OP_ROOM_ADD-->ROOM_ADD

        OP_ROOM_DEL-->ROOM_QUERY
        OP_ROOM_DEL-->ROOM_DEL

        OP_ROOM_MOD-->ROOM_QUERY
        OP_ROOM_MOD-->ROOM_INFO
    end

    subgraph ui
        direction LR
        MENU["`menu()`"]-->OP1["`op_guest_list()`"]
        MENU-->OP2["`op_room_list()`"]
        OP1-->OP_GUEST
        OP2-->OP_ROOM
    end

    init-->ui
    ui-->END
```

# 致谢

感谢我的组员帮我写了一些代码，还有一个好看的 ui 界面。

可视化因为还没有玩明白所以先不加了。

感谢 fitten code 救我狗命，不然写 `<ctime>` 库的时候我就原地爆炸了。

感谢 github 为了种种原因把默认分支改成 main，让我 `push master` 的时候各种出错最后删了十几次库。