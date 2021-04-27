#include <iostream>
#include <fstream>
#include <optional>
#include <vector>
#include <tuple>
#include <string>
#include <variant>
#include <iomanip>
#include <algorithm>

struct Person
{
    Person() {};
    Person(std::string sn, std::string n, std::string pn) : surname(sn), name(n), patronymic(pn) { }
    std::string surname;
    std::string name;
    std::optional<std::string> patronymic;
};

std::ostream& operator<<(std::ostream& os, const Person& person) {
    std::cout << std::setw(10) << person.surname << " " << std::setw(10) << person.name << " " << std::setw(20) << person.patronymic.value_or("**");
    return os;
}

bool operator==(const Person& p1, const Person& p2)
{
    return tie(p1.surname, p1.name, p1.patronymic) == tie(p2.surname, p2.name, p2.patronymic); 
}

bool operator<(const Person& p1, const Person& p2)
{
    return tie(p1.surname, p1.name, p1.patronymic) < tie(p2.surname, p2.name, p2.patronymic);
}


struct PhoneNumber {
    int pn_country=0;
    int pn_town=0;
    std::string pn_number;
    std::optional<int> pn_extention;
};

std::ostream& operator<<(std::ostream& os, const PhoneNumber& phoneNumber) {
    std::cout << "+" << phoneNumber.pn_country << "(" << phoneNumber.pn_town << ")" << phoneNumber.pn_number;
    if (phoneNumber.pn_extention.has_value())
        std::cout << " " << phoneNumber.pn_extention.value();
    return os;
}



class PhoneBook
{
private:
    std::vector<std::pair<Person, PhoneNumber>> phoneBook;
public:
    PhoneBook(std::ifstream& in) {
        while (!in.eof())
        {
            Person person;
            PhoneNumber phoneNumber;
            std::string s;
            in >> person.surname >> person.name >> s;
            if (s == "-")
                person.patronymic = std::nullopt;
            else person.patronymic = s;
            in >> phoneNumber.pn_country >> phoneNumber.pn_town >> phoneNumber.pn_number >> s;
            if (s == "-")
                phoneNumber.pn_extention = std::nullopt;
            else phoneNumber.pn_extention = stoi(s);
            phoneBook.push_back({ person, phoneNumber });
        }

    }

    friend std::ostream& operator<<(std::ostream& out, const PhoneBook& pb) {
        for (const auto& [person, phoneNumber] : pb.phoneBook) {
            out << person << std::setw(20) << phoneNumber << std::endl;
        }
        return out;
    }


    void SortByName() {
        std::sort(phoneBook.begin(), phoneBook.end(),
            []
        (const auto& record1, const auto& record2) {
                return record1.first < record2.first;
            });
    }

    void SortByPhone() {
        std::sort(phoneBook.begin(), phoneBook.end(),
            []
        (const auto& record1, const auto& record2) {
                const PhoneNumber& p1 = record1.second;
                const PhoneNumber& p2 = record2.second;
                return  tie(p1.pn_country, p1.pn_town, p1.pn_number, p1.pn_extention) < tie(p2.pn_country, p2.pn_town, p2.pn_number, p2.pn_extention);
            });
    };

    std::tuple<std::string, PhoneNumber> GetPhoneNumber(std::string surname) {
        PhoneNumber phoneNumber;
        std::string result = "";
        int counter = 0;
        std::for_each(phoneBook.begin(), phoneBook.end(), [&](const auto& record) {
            if (record.first.surname == surname) {
                phoneNumber = record.second;
                ++counter;
            }
            });
        if (counter == 0)
            result = "not found";
        else if (counter > 1)
            result = "found more than 1";
        return std::make_tuple(result, phoneNumber);
    }

    void ChangePhoneNumber(const Person& person, const PhoneNumber& newNumber) {
        auto find_record = std::find_if(phoneBook.begin(), phoneBook.end(), [&person](const auto& record) {
            return record.first == person;
            });
        if (find_record != phoneBook.end())
            find_record->second = newNumber;
    }
};



int main()
{
    std::ifstream file("PhoneBook.txt"); // путь к файлу PhoneBook.txt
    PhoneBook book(file);
    std::cout << book;

    std::cout << "------SortByPhone-------" << std::endl;
    book.SortByPhone();
    std::cout << book;

    std::cout << "------SortByName--------" << std::endl;
    book.SortByName();
    std::cout << book;

    std::cout << "-----GetPhoneNumber-----" << std::endl;
    // лямбда функция, которая принимает фамилию и выводит номер телефона этого человека, либо строку с ошибкой
    auto print_phone_number = [&book](const std::string& surname) {
        std::cout << surname << "\t";
        auto answer = book.GetPhoneNumber(surname);
        if (std::get<0>(answer).empty())
            std::cout << std::get<1>(answer);
        else
            std::cout << std::get<0>(answer);
        std::cout << std::endl;
    };

    // вызовы лямбды
    print_phone_number("Ivanov");
    print_phone_number("Petrov");


    std::cout << "----ChangePhoneNumber----" << std::endl;
    book.ChangePhoneNumber(Person{ "Kotov", "Vasilii", "Eliseevich" }, PhoneNumber{ 7, 123, "15344458", std::nullopt });
    book.ChangePhoneNumber(Person{ "Mironova", "Margarita", "Vladimirovna" }, PhoneNumber{ 16, 465, "9155448", 13 });
    std::cout << book;

}

