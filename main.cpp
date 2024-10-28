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

// �� time_t ����ת��Ϊ�ַ�����ʽ
string time_t_to_string(time_t t) {
    if (t == MAX_TIME) { return "NOT_LEAVE"; }
    char buf[2005];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&t));
    return string(buf);
}
 
// ���ַ�����ʽ������ʱ��ת��Ϊ time_t ����
time_t string_to_time_t(string dateTimeStr) {
    istringstream ss(dateTimeStr);
    tm tm = {};
    ss >> get_time(&tm, "%Y-%m-%d %H:%M:%S");
    if (ss.fail()) {
        return MAX_TIME;
    }
    // �� tm �ṹת��Ϊ time_t
    return mktime(&tm);
}

// ������֤�Ƿ�Ϸ�
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
        cout << "����ţ�" << ID << endl;
        cout << "�������ͣ�" << type << endl;
        cout << "����۸�" << cost << endl;
        cout << "�Ƿ��ѱ�ռ�ã�" << (isOccupied ? "��" : "��") << endl;
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
        cout << "�������֤�ţ�" << ID << endl;
        cout << "����������" << name << endl;
        cout << "��סʱ�䣺" << time_t_to_string(checkin_time) << endl;
        cout << "���ʱ�䣺" << (checkout_time == MAX_TIME ? "δ���" : time_t_to_string(checkout_time)) << endl;
        cout << "ס�޷��ã�" << cost << endl;
        cout << "ס�޷��䣺" << (room == nullptr ? "δָ��" : room->data.ID) << endl;
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
        cout << "�޷����ļ�: " << selectedFileName << endl;
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
        cout << "�޷����ļ�: " << selectedFileName << "������ʧ�ܣ�" << endl;
        return;
    }
    for (auto p=head->nxt; p!= nullptr; p = p->nxt) {
        p->data.output_info(outputFile);
    }
    // �ر��ļ�  
    outputFile.close();  
    cout << "��Ϣ�ѳɹ����浽�ļ�: " << selectedFileName << endl;  
}

void menu() {
	cout << "      \033[33m@��@��@��@��@��@��@��@��@��@��@��@��@��@��@��@��\033[0m" << endl;
	cout << "      \033[33m|\033[0m     ��ӭʹ�þƵ����ϵͳ     \033[33m|\033[0m" << endl;
	cout << "   \033[3mWelcome to the hotel management system\033[0m" << endl;
	cout << "       \033[33m******************************" << endl;
	cout << "      \033[33m|\033[0m                              \033[33m|\033[0m" << "     #   #" << endl;
	cout << "      \033[33m|\033[0m      1.������Ϣ              \033[33m|\033[0m" << "	  # # # #" << endl;
	cout << "      \033[33m|\033[0m                              \033[33m|\033[0m" << "	  # # # #" << endl;
	cout << "      \033[33m|\033[0m      2.�ͷ���Ϣ              \033[33m|\033[0m" << "	  # ### #" << endl;
	cout << "      \033[33m|\033[0m                              \033[33m|\033[0m" << "   #       #" << endl;
	cout << "      \033[33m|\033[0m      3.�˳�ϵͳ              \033[33m|\033[0m" << "   # #   # #" << endl;
	cout << "      \033[33m|\033[0m                              \033[33m|\033[0m" << "    #######" << endl;
	cout << "      \033[33m********************************\033[0m" << endl;
	cout << "��ѡ��";
}

void command() {
    cout << "          <��ѡ������Ҫִ�е�ָ��>    " << endl;
    cout << "      ================================" << endl;
    cout << "                 1.��ѯ��Ϣ           " << endl;
    cout << "                                      " << endl;
    cout << "                 2.������Ϣ           " << endl;
    cout << "                                      " << endl;
    cout << "                 3.ɾ����Ϣ           " << endl;
    cout << "                                      " << endl;
    cout << "                 4.�޸���Ϣ           " << endl;
    cout << "      ����������������������������������������������������������������" << endl;
    cout << "��ѡ��";
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
    cout << "�������ѯ��Ŀ��1.ȫ��������Ϣ 2.ָ��������Ϣ����";
    int item_id;
    cin >> item_id;
    if (item_id == 1) {
        cout << "---------------------------------------------------" << endl;
        for (auto p=gl->nxt; p!= nullptr; p = p->nxt) {
            p->data.output_info();
            cout << "---------------------------------------------------" << endl;
        }
    } else if (item_id == 2) {
        cout << "������Ҫ��ѯ�Ŀ������֤�ţ�";
        string guest_id;
        cin >> guest_id;
        auto p = find<Guest>(gl, Guest{guest_id, "", 0, 0, 0, nullptr});
        if (p == nullptr) {
            cout << "�ÿ��˲����ڣ�" << endl;
        } else {
            p->data.output_info();
        }
    }
}

void add_guest()
{
    string guest_id, guest_name, room_id;
    cout << "������Ҫ��ӵĿ��˵����֤�ţ�";
    cin >> guest_id;
    if (!ID_is_valid(guest_id)) {
        cout << "���֤���Ϸ���" << endl;
        return;
    }
    auto p = find<Guest>(gl, Guest{guest_id, "", 0, 0, 0, nullptr});
    if (p != nullptr) { cout << "�ÿ����Ѵ��ڣ�" << endl; }
    else {
        cout << "������Ҫ��ӵĿ��˵�������";
        cin >> guest_name;
        cout << "������Ҫ��ӵĿ��˵���ס���ţ�";
        cin >> room_id;
        auto x = find_room(rl, room_id);
        if (x == nullptr) {
            cout << "�÷��䲻���ڣ�" << endl;
        } else if (x->data.isOccupied) {
            cout << "�÷����ѱ�ռ�ã�" << endl;
        } else {
            add<Guest>(gl, Guest{guest_id, guest_name, time(0), MAX_TIME, 0, x});
            x->data.isOccupied = true;
            cout << "������Ϣ�ѳɹ�������" << endl;
        }
    }
}

void del_guest()
{
    cout << "������Ҫɾ���Ŀ������֤�ţ�";
    string guest_id;
    cin >> guest_id;
    auto p = find(gl, Guest{guest_id, "", 0, 0, 0, nullptr});
    if (del<Guest>(gl, Guest{guest_id, "", 0, 0, 0, nullptr})) {
        p->data.room->data.isOccupied = false;
        cout << "������Ϣ�ѳɹ�ɾ����" << endl;
    } else {
        cout << "�ÿ��˲����ڣ�" << endl;
    }
}

void modify_guest()
{
    bool success = false;
    cout << "������Ҫ�޸ĵĿ������֤�ţ�";
    string guest_id;
    cin >> guest_id;
    auto p = find<Guest>(gl, Guest{guest_id, "", 0, 0, 0, nullptr});
    if (p == nullptr) {
        cout << "�ÿ��˲����ڣ�" << endl;
    } else {
        cout << "������Ҫ�޸ĵ���Ŀ�ţ�1.���� 2.�Ƿ��뿪 3.��סʱ�� 4.���ʱ�� 5.ס�޷��䣩��";
        string item_id;
        cin >> item_id;
        if (item_id == "1") {
            cout << "�������µ�������";
            string new_name;
            cin >> new_name;
            p->data.name = new_name;
            success = true;
        } else if (item_id == "2") {
            if (p->data.checkout_time != MAX_TIME) {
                auto x = find_room(rl, p->data.room->data.ID);
                if (x == nullptr) {
                    cout << "ԭ���䲻���ڣ�" << endl;
                } else if (x->data.isOccupied) {
                    cout << "ԭ�����ѱ�ռ�ã�" << endl;
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
            cout << "�������µ���סʱ�䣨��ʽ��YYYY-MM-DD HH:MM:SS����";
            string new_checkin_time;
            cin.ignore();
            getline(cin, new_checkin_time);
            time_t new_checkin_time_t = string_to_time_t(new_checkin_time);
            if (new_checkin_time_t == MAX_TIME) {
                cout << "ʱ���ʽ����ȷ" << std::endl;
            } else {
                p->data.checkin_time = new_checkin_time_t;
                p->data.calc_cost();
                success = true;
            }
        } else if (item_id == "4") {
            cout << "�������µ����ʱ�䣨��ʽ��YYYY-MM-DD HH:MM:SS����";
            string new_checkout_time;
            cin.ignore();
            getline(cin, new_checkout_time);
            time_t new_checkout_time_t = string_to_time_t(new_checkout_time);
            if (new_checkout_time_t == MAX_TIME) {
                cout << "ʱ���ʽ����ȷ" << std::endl;
            } else {
                p->data.checkout_time = new_checkout_time_t;
                p->data.calc_cost();
                success = true;
            }
        } else if (item_id == "5") {
            cout << "�������µ�ס�޷���ţ�";
            string new_room_id;
            cin >> new_room_id;
            auto x = find_room(rl, new_room_id);
            if (x == nullptr) {
                cout << "�÷��䲻���ڣ�" << endl;
            } else if (x->data.isOccupied) {
                cout << "�÷����ѱ�ռ�ã�" << endl;
            } else {
                p->data.room->data.isOccupied = false;
                p->data.room = x;
                x->data.isOccupied = true;
                success = true;
            }
        }
        if (success) {
            cout << "�޸ĳɹ����޸ĺ����Ϣ���£�" << endl;
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
        cout << "����������������룡" << endl;
    }
}

void query_room()
{
    cout << "�������ѯ��Ŀ��1.ȫ��������Ϣ 2.ָ��������Ϣ 3.ָ���۸����䣩��";
    int item_id;
    cin >> item_id;
    if (item_id == 1) {
    cout << "---------------------------------------------------" << endl;
        for (auto p=rl->nxt; p!= nullptr; p = p->nxt) {
            p->data.output_info();
            cout << "---------------------------------------------------" << endl;
        }
    } else if (item_id == 2) {
        cout << "������Ҫ��ѯ�ķ���ţ�";
        string room_id;
        cin >> room_id;
        auto p = find<Room>(rl, Room{room_id, "", 0, false});
        if (p == nullptr) {
            cout << "�÷��䲻���ڣ�" << endl;
        } else {
            p->data.output_info();
        }
    } else if (item_id == 3) {
        cout << "������۸����䣨��ʽ��min-max����";
        string price_range;
        cin >> price_range;
        double min_price, max_price;
        int pos = price_range.find("-");
        if (pos == string::npos) {
            cout << "�����ʽ����" << endl;
        } else {
            min_price = stod(price_range.substr(0, pos));
            max_price = stod(price_range.substr(pos+1));
            if (min_price > max_price) { cout << "�����ʽ����" << endl; }
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
        cout << "����������������룡" << endl;
    }
}

void add_room()
{
    cout << "������Ҫ�����ķ�����Ϣ��" << endl;
    string room_id, room_type, cost;
    cout << "�����뷿��ţ�";
    cin >> room_id;
    auto p = find(rl, Room{room_id, "", 0, false});
    if (p != nullptr) { cout << "�÷����Ѵ��ڣ�" << endl; }
    else {
        cout << "�����뷿�����ͣ�";
        cin >> room_type;
        cout << "�����뷿��۸�";
        cin >> cost;
        try {
            add<Room>(rl, Room{room_id, room_type, stod(cost), false});
            cout << "������Ϣ�ѳɹ�������" << endl;                
        } catch (...) { cout << "�۸��ʽ����" << endl; }
    }
}

void del_room()
{
    cout << "������Ҫɾ���ķ���ID��";
    string room_id;
    cin >> room_id;
    if (del<Room>(rl, Room{room_id, "", 0, false})) {
        cout << "������Ϣ�ѳɹ�ɾ����" << endl;
    } else {
        cout << "�÷��䲻���ڣ�" << endl;
    }
}

void modify_room()
{
    cout << "������Ҫ�޸ĵķ���ţ�";
    string room_id;
    cin >> room_id;
    auto p = find(rl, Room{room_id, "", 0, false});
    if (p == nullptr) {
        cout << "�÷��䲻���ڣ�" << endl;
    } else {
        bool success = false;
        cout << "������Ҫ�޸ĵ���Ŀ�ţ�1.���� 2.�۸񣩣�";
        string item_id;
        cin >> item_id;
        if (item_id == "1") {
            cout << "�������µ����ͣ�";
            string new_type;
            cin >> new_type;
            p->data.type = new_type;
            success = true;
        } else if (item_id == "2") {
            // WARNING :: �޸ļ۸���û��Ļ����Ƿ���Ҫ�ı䣿
            cout << "�������µļ۸�";
            string new_cost;
            cin >> new_cost;
            try {
                p->data.cost = stod(new_cost);
                success = true;
            } catch (...) { cout << "�۸��ʽ����" << endl; }
        }
        if (success) {
            cout << "�޸ĳɹ����޸ĺ����Ϣ���£�" << endl;
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
            cout << "�˳�ϵͳ��" << endl;
            break;
        } else {
            cout << "����������������룡" << endl;
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
