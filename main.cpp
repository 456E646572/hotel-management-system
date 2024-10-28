#include <iostream>
#include <ctime>
#include <string>
#include <fstream>
#include <limits>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <stdexcept>
#include <memory>

using namespace std;

const string ROOMFILE = "room.txt";
const string GUESTFILE = "guest.txt";
const string NULL_ROOM_ID = "0";
const time_t MAX_TIME = -1;

// 将 time_t 类型转换为字符串形式
string time_t_to_string(time_t t) {
    if (t == MAX_TIME) { return "NOT_LEAVE"; }
    char buf[2005];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&t));
    return string(buf);
}
 
// 将字符串形式的日期时间转换为 time_t 类型
time_t string_to_time_t(string dateTimeStr) {
    istringstream ss(dateTimeStr);
    tm tm = {};
    ss >> get_time(&tm, "%Y-%m-%d %H:%M:%S");
    if (ss.fail()) {
        return MAX_TIME;
    }
    // 将 tm 结构转换为 time_t
    return mktime(&tm);
}

// 检查身份证是否合法
bool ID_is_valid(string ID) {
    for (char c: ID) {
        if ('0'<=c&&c<='9' || c=='X') { continue; }
        else { return false; }
    }
    return ID.length()==18;
}

template<typename T>
struct node {
    T data;
    shared_ptr<node<T>> nxt;
};

struct Room {
    string ID;
    string type;
    double cost;
    bool isOccupied=false;
    bool operator == (const Room& other) const {
        return ID == other.ID;
    }
    void add_attribute(int att_id, string att) {
        switch (att_id) {
            case 0: ID = att; break;
            case 1: type = att; break;
            case 2: cost = stod(att); break;
            default: break;
        }
    }
    void output_info() {
        cout << "房间号：" << ID << endl;
        cout << "房间类型：" << type << endl;
        cout << "房间价格：" << cost << endl;
        cout << "是否已被占用：" << (isOccupied ? "是" : "否") << endl;
    }
    void output_info(ofstream& f) {
        f << ID << " " << type << " " << cost << endl;
    }
};
using RoomList = shared_ptr<node<Room>>;
RoomList rl = make_shared<node<Room>>();

RoomList find_room (RoomList rl, string ID) {
    if (ID == NULL_ROOM_ID) { return nullptr; }
    for (auto p = rl->nxt; p!= nullptr; p = p->nxt) {
        if (p->data.ID == ID) {
            return p;
        }
    }
    return nullptr;
}

struct Guest {
    string ID;
    string name;
    time_t checkin_time;
    time_t checkout_time;
    double cost = 0;
    RoomList room=nullptr;
    bool operator == (const Guest& other) const {
        return ID == other.ID;
    }
    void add_attribute(int att_id, string att) {
        switch (att_id) {
            case 0: ID = att; break;
            case 1: name = att; break;
            case 2: checkin_time = stod(att); break;
            case 3: checkout_time = stod(att); break;
            case 4: cost = stod(att); break;
            case 5: room = find_room(rl, att); break;
            default: break;
        }
    }
    void calc_cost() {
        if (checkout_time != MAX_TIME) {
            cost = (checkout_time-checkin_time)/3600.0*(room->data.cost);
        }
    }
    void output_info() {
        cout << "客人身份证号：" << ID << endl;
        cout << "客人姓名：" << name << endl;
        cout << "入住时间：" << time_t_to_string(checkin_time) << endl;
        cout << "离店时间：" << (checkout_time == MAX_TIME ? "未离店" : time_t_to_string(checkout_time)) << endl;
        cout << "住宿费用：" << cost << endl;
        cout << "住宿房间：" << (room == nullptr ? "未指定" : room->data.ID) << endl;
    }
    void output_info(ofstream& f) {
        f << ID << " " << name << " " << checkin_time << " " << checkout_time << " " << cost << " " << (room == nullptr? NULL_ROOM_ID : room->data.ID) << endl;
    }
};
using GuestList = shared_ptr<node<Guest>>;
GuestList gl = make_shared<node<Guest>>();

template <typename T>
bool add(shared_ptr<node<T>> head, T data) {
    auto p = head;
    for (; p->nxt != nullptr; p = p->nxt) {
        if (p->nxt->data == data) { return false;}
    }
    p->nxt = make_shared<node<T>>();
    p->nxt->data = data;
    p->nxt->nxt = nullptr;
    return true;
}

template <typename T>
bool del(shared_ptr<node<T>> head, T data) {
    for (auto p=head; p->nxt!= nullptr; p = p->nxt) {
        if (p->nxt->data == data) {
            p->nxt = p->nxt->nxt;
            return true;
        } 
    }
    return false;
}

template <typename T>
shared_ptr<node<T>> find(shared_ptr<node<T>> head, const T& data) {
    for (auto p=head; p->nxt!= nullptr; p = p->nxt) {
        if (p->nxt->data == data) {
            return p->nxt;
        }
    }
    return nullptr;
}

template<typename T>
void read(string selectedFileName, shared_ptr<node<T>> head) {
    ifstream file(selectedFileName);
    if (!file.is_open()) {  
        cout << "无法打开文件: " << selectedFileName << endl;
        exit(0);
    }
    string line;
    while (getline(file, line)) {
        T data = T();
        int att_id = 0;
        string t;
        for (char c: line) {
            if (c == ' ') { data.add_attribute(att_id, t); att_id++; t = ""; }
            else { t += c; }
        }
        data.add_attribute(att_id, t);
        add<T>(head, data);
    }
    file.close();
}

template<typename T>
void write(string selectedFileName, shared_ptr<node<T>> head) {
    ofstream outputFile(selectedFileName);  
    if (!outputFile.is_open()) {  
        cout << "无法打开文件: " << selectedFileName << "，保存失败！" << endl;
        return;
    }
    for (auto p=head->nxt; p!= nullptr; p = p->nxt) {
        p->data.output_info(outputFile);
    }
    // 关闭文件  
    outputFile.close();  
    cout << "信息已成功保存到文件: " << selectedFileName << endl;  
}

void menu() {
	cout << "      \033[33m@—@—@—@—@—@—@—@—@—@—@—@—@—@—@—@—\033[0m" << endl;
	cout << "      \033[33m|\033[0m     欢迎使用酒店管理系统     \033[33m|\033[0m" << endl;
	cout << "   \033[3mWelcome to the hotel management system\033[0m" << endl;
	cout << "       \033[33m******************************" << endl;
	cout << "      \033[33m|\033[0m                              \033[33m|\033[0m" << "     #   #" << endl;
	cout << "      \033[33m|\033[0m      1.客人信息              \033[33m|\033[0m" << "	  # # # #" << endl;
	cout << "      \033[33m|\033[0m                              \033[33m|\033[0m" << "	  # # # #" << endl;
	cout << "      \033[33m|\033[0m      2.客房信息              \033[33m|\033[0m" << "	  # ### #" << endl;
	cout << "      \033[33m|\033[0m                              \033[33m|\033[0m" << "   #       #" << endl;
	cout << "      \033[33m|\033[0m      3.退出系统              \033[33m|\033[0m" << "   # #   # #" << endl;
	cout << "      \033[33m|\033[0m                              \033[33m|\033[0m" << "    #######" << endl;
	cout << "      \033[33m********************************\033[0m" << endl;
	cout << "请选择：";
}

void command() {
    cout << "          <请选择您所要执行的指令>    " << endl;
    cout << "      ================================" << endl;
    cout << "                 1.查询信息           " << endl;
    cout << "                                      " << endl;
    cout << "                 2.新增信息           " << endl;
    cout << "                                      " << endl;
    cout << "                 3.删除信息           " << endl;
    cout << "                                      " << endl;
    cout << "                 4.修改信息           " << endl;
    cout << "      ————————————————————————————————" << endl;
    cout << "请选择：";
}

void init() {
    read<Room>(ROOMFILE, rl);
    read<Guest>(GUESTFILE, gl);
    for (auto p=gl->nxt; p!= nullptr; p = p->nxt) {
        if (p->data.checkout_time == MAX_TIME) {
            p->data.room->data.isOccupied = true;
        }
    }
}

void query_guest()
{
    cout << "请输入查询条目（1.全部客人信息 2.指定客人信息）：";
    int item_id;
    cin >> item_id;
    if (item_id == 1) {
        cout << "---------------------------------------------------" << endl;
        for (auto p=gl->nxt; p!= nullptr; p = p->nxt) {
            p->data.output_info();
            cout << "---------------------------------------------------" << endl;
        }
    } else if (item_id == 2) {
        cout << "请输入要查询的客人身份证号：";
        string guest_id;
        cin >> guest_id;
        auto p = find<Guest>(gl, Guest{guest_id, "", 0, 0, 0, nullptr});
        if (p == nullptr) {
            cout << "该客人不存在！" << endl;
        } else {
            p->data.output_info();
        }
    }
}

void add_guest()
{
    string guest_id, guest_name, room_id;
    cout << "请输入要添加的客人的身份证号：";
    cin >> guest_id;
    if (!ID_is_valid(guest_id)) {
        cout << "身份证不合法！" << endl;
        return;
    }
    auto p = find<Guest>(gl, Guest{guest_id, "", 0, 0, 0, nullptr});
    if (p != nullptr) { cout << "该客人已存在！" << endl; }
    else {
        cout << "请输入要添加的客人的姓名：";
        cin >> guest_name;
        cout << "请输入要添加的客人的入住房号：";
        cin >> room_id;
        auto x = find_room(rl, room_id);
        if (x == nullptr) {
            cout << "该房间不存在！" << endl;
        } else if (x->data.isOccupied) {
            cout << "该房间已被占用！" << endl;
        } else {
            add<Guest>(gl, Guest{guest_id, guest_name, time(0), MAX_TIME, 0, x});
            x->data.isOccupied = true;
            cout << "客人信息已成功新增！" << endl;
        }
    }
}

void del_guest()
{
    cout << "请输入要删除的客人身份证号：";
    string guest_id;
    cin >> guest_id;
    auto p = find(gl, Guest{guest_id, "", 0, 0, 0, nullptr});
    if (del<Guest>(gl, Guest{guest_id, "", 0, 0, 0, nullptr})) {
        p->data.room->data.isOccupied = false;
        cout << "客人信息已成功删除！" << endl;
    } else {
        cout << "该客人不存在！" << endl;
    }
}

void modify_guest()
{
    bool success = false;
    cout << "请输入要修改的客人身份证号：";
    string guest_id;
    cin >> guest_id;
    auto p = find<Guest>(gl, Guest{guest_id, "", 0, 0, 0, nullptr});
    if (p == nullptr) {
        cout << "该客人不存在！" << endl;
    } else {
        cout << "请输入要修改的条目号（1.姓名 2.是否离开 3.入住时间 4.离店时间 5.住宿房间）：";
        string item_id;
        cin >> item_id;
        if (item_id == "1") {
            cout << "请输入新的姓名：";
            string new_name;
            cin >> new_name;
            p->data.name = new_name;
            success = true;
        } else if (item_id == "2") {
            if (p->data.checkout_time != MAX_TIME) {
                auto x = find_room(rl, p->data.room->data.ID);
                if (x == nullptr) {
                    cout << "原房间不存在！" << endl;
                } else if (x->data.isOccupied) {
                    cout << "原房间已被占用！" << endl;
                } else {
                    p->data.checkout_time = MAX_TIME;
                    p->data.cost = 0;
                    p->data.room->data.isOccupied = true;
                    success = true;
                }
            } else {
                p->data.checkout_time = time(0);
                p->data.calc_cost();
                p->data.room->data.isOccupied = false;
                success = true;
            }
        } else if (item_id == "3") {
            cout << "请输入新的入住时间（格式：YYYY-MM-DD HH:MM:SS）：";
            string new_checkin_time;
            cin.ignore();
            getline(cin, new_checkin_time);
            time_t new_checkin_time_t = string_to_time_t(new_checkin_time);
            if (new_checkin_time_t == MAX_TIME) {
                cout << "时间格式不正确" << std::endl;
            } else {
                p->data.checkin_time = new_checkin_time_t;
                p->data.calc_cost();
                success = true;
            }
        } else if (item_id == "4") {
            cout << "请输入新的离店时间（格式：YYYY-MM-DD HH:MM:SS）：";
            string new_checkout_time;
            cin.ignore();
            getline(cin, new_checkout_time);
            time_t new_checkout_time_t = string_to_time_t(new_checkout_time);
            if (new_checkout_time_t == MAX_TIME) {
                cout << "时间格式不正确" << std::endl;
            } else {
                p->data.checkout_time = new_checkout_time_t;
                p->data.calc_cost();
                success = true;
            }
        } else if (item_id == "5") {
            cout << "请输入新的住宿房间号：";
            string new_room_id;
            cin >> new_room_id;
            auto x = find_room(rl, new_room_id);
            if (x == nullptr) {
                cout << "该房间不存在！" << endl;
            } else if (x->data.isOccupied) {
                cout << "该房间已被占用！" << endl;
            } else {
                p->data.room->data.isOccupied = false;
                p->data.room = x;
                x->data.isOccupied = true;
                success = true;
            }
        }
        if (success) {
            cout << "修改成功！修改后的信息如下：" << endl;
            p->data.output_info();
        }
    }
}

void op_guest_list() {
    command();
    string command_id;
    cin >> command_id;
    cout << "---------------------------------------------------" << endl;
    if (command_id == "1") {
        query_guest();
    } else if (command_id == "2") {
        add_guest();
    } else if (command_id == "3") {
        del_guest();
    } else if (command_id == "4") {
        modify_guest();
    } else {
        cout << "输入错误，请重新输入！" << endl;
    }
}

void query_room()
{
    cout << "请输入查询条目（1.全部房间信息 2.指定房间信息 3.指定价格区间）：";
    int item_id;
    cin >> item_id;
    if (item_id == 1) {
    cout << "---------------------------------------------------" << endl;
        for (auto p=rl->nxt; p!= nullptr; p = p->nxt) {
            p->data.output_info();
            cout << "---------------------------------------------------" << endl;
        }
    } else if (item_id == 2) {
        cout << "请输入要查询的房间号：";
        string room_id;
        cin >> room_id;
        auto p = find<Room>(rl, Room{room_id, "", 0, false});
        if (p == nullptr) {
            cout << "该房间不存在！" << endl;
        } else {
            p->data.output_info();
        }
    } else if (item_id == 3) {
        cout << "请输入价格区间（格式：min-max）：";
        string price_range;
        cin >> price_range;
        double min_price, max_price;
        int pos = price_range.find("-");
        if (pos == string::npos) {
            cout << "输入格式错误！" << endl;
        } else {
            min_price = stod(price_range.substr(0, pos));
            max_price = stod(price_range.substr(pos+1));
            if (min_price > max_price) { cout << "输入格式错误！" << endl; }
            else {
                cout << "---------------------------------------------------" << endl;
                for (auto p=rl->nxt; p!= nullptr; p = p->nxt) {
                    if (p->data.cost >= min_price && p->data.cost <= max_price) {
                        p->data.output_info();
                        cout << "---------------------------------------------------" << endl;
                    }
                }
            }
        }
    } else {
        cout << "输入错误，请重新输入！" << endl;
    }
}

void add_room()
{
    cout << "请输入要新增的房间信息：" << endl;
    string room_id, room_type, cost;
    cout << "请输入房间号：";
    cin >> room_id;
    auto p = find(rl, Room{room_id, "", 0, false});
    if (p != nullptr) { cout << "该房间已存在！" << endl; }
    else {
        cout << "请输入房间类型：";
        cin >> room_type;
        cout << "请输入房间价格：";
        cin >> cost;
        try {
            add<Room>(rl, Room{room_id, room_type, stod(cost), false});
            cout << "房间信息已成功新增！" << endl;                
        } catch (...) { cout << "价格格式错误！" << endl; }
    }
}

void del_room()
{
    cout << "请输入要删除的房间ID：";
    string room_id;
    cin >> room_id;
    if (del<Room>(rl, Room{room_id, "", 0, false})) {
        cout << "房间信息已成功删除！" << endl;
    } else {
        cout << "该房间不存在！" << endl;
    }
}

void modify_room()
{
    cout << "请输入要修改的房间号：";
    string room_id;
    cin >> room_id;
    auto p = find(rl, Room{room_id, "", 0, false});
    if (p == nullptr) {
        cout << "该房间不存在！" << endl;
    } else {
        bool success = false;
        cout << "请输入要修改的条目号（1.类型 2.价格）：";
        string item_id;
        cin >> item_id;
        if (item_id == "1") {
            cout << "请输入新的类型：";
            string new_type;
            cin >> new_type;
            p->data.type = new_type;
            success = true;
        } else if (item_id == "2") {
            // WARNING :: 修改价格后用户的花费是否需要改变？
            cout << "请输入新的价格：";
            string new_cost;
            cin >> new_cost;
            try {
                p->data.cost = stod(new_cost);
                success = true;
            } catch (...) { cout << "价格格式错误！" << endl; }
        }
        if (success) {
            cout << "修改成功！修改后的信息如下：" << endl;
            p->data.output_info();
        }
    }
}

void op_room_list() {
    command();
    string command_id;
    cin >> command_id;
    cout << "---------------------------------------------------" << endl;
    if (command_id == "1") {
        query_room();
    } else if (command_id == "2") {
        add_room();
    } else if (command_id == "3") {
        del_room();
    } else if (command_id == "4") {
        modify_room();
    }
}

void ui() {
    while (1) {
        system("cls");
        menu();
        string file_id;
        cin >> file_id;
        if (file_id == "1") {
            system("cls"); op_guest_list();
        } else if (file_id == "2") {
            system("cls"); op_room_list();
        } else if (file_id == "3") {
            write<Room>(ROOMFILE, rl);
            write<Guest>(GUESTFILE, gl);
            cout << "退出系统！" << endl;
            break;
        } else {
            cout << "输入错误，请重新输入！" << endl;
        }
        system("pause");
    }
    system("pause");
}

int main() {
    init();
    ui();
    return 0;
}
