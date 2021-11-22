#include <iostream>
#include <string>
#include <optional>
#include <tuple>
#include <vector>
#include <fstream>
#include <algorithm>


struct Person{
    std::string surname;
    std::string name;
    std::optional<std::string> patronymic;
};

std::ostream &operator<<(std::ostream &out, const Person &person){
    if(person.patronymic.has_value())
        return out << person.surname << " " << person.name << " " << person.patronymic.value();
    else{
        return out << person.surname << " " << person.name;
    }
}

bool operator<(const Person &p1, const Person &p2){
    return tie(p1.surname, p1.name, p1.patronymic) < tie(p2.surname, p2.name, p2.patronymic);
}

bool operator==(const Person &p1, const Person &p2){
    return tie(p1.surname, p1.name, p1.patronymic) == tie(p2.surname, p2.name, p2.patronymic);
}

struct PhoneNumber{
    int code_country;
    int code_city;
    int phone_number;
    std::optional<int> additional_code;
};

std::ostream &operator<<(std::ostream &out, const PhoneNumber &phoneNumber){
    if(phoneNumber.additional_code.has_value())
        return out << "+" << phoneNumber.code_country << "(" << phoneNumber.code_city << ")"
                   << phoneNumber.phone_number << " " << phoneNumber.additional_code.value();
    else{
        return out << "+" << phoneNumber.code_country << "(" << phoneNumber.code_city << ")"
                   << phoneNumber.phone_number;
    }
}

bool operator<(const PhoneNumber &p1, const PhoneNumber &p2){
    return tie(p1.code_country, p1.code_city, p1.phone_number, p1.additional_code)
         < tie(p2.code_country, p2.code_city, p2.phone_number, p2.additional_code);
}

bool operator==(const PhoneNumber &p1, const PhoneNumber &p2){
    return tie(p1.code_country, p1.code_city, p1.phone_number, p1.additional_code)
        == tie(p2.code_country, p2.code_city, p2.phone_number, p2.additional_code);
}



class PhoneBook{
    std::vector <std::pair<Person, PhoneNumber>> vec;

public:
    PhoneBook(std::ifstream &file){
        if(!file){
            std::cout << "File not opened!";
        }else{
            std::string s;
            std::string token{"+"};
            size_t pos = 0;
            size_t token_size = token.size();
            std::string tmp;
            int count;



            while (getline(file,s)){
                Person person;
                PhoneNumber phoneNumber;
                size_t base_str_size = s.size();
                while(pos < base_str_size) {
                    tmp = tmp.assign(s, pos, s.find(token, pos) - pos);
                    if(tmp.size() > 0){
                        if(pos == 0){
                            std::string tokenF{" "};
                            size_t posF = 0;
                            size_t token_sizeF = tokenF.size();
                            std::string tmpF;
                            size_t base_str_sizeF = tmp.size();
                            count = 0;
                            while(posF < base_str_sizeF){
                                tmpF = tmpF.assign(tmp, posF, tmp.find(tokenF, posF) - posF);
                                ++count;
                                if(count == 1){
                                    person.surname = tmpF;
                                }else if(count == 2){
                                    person.name = tmpF;
                                }else if(count == 3){
                                    person.patronymic = tmpF;
                                }
                                posF += tmpF.size() + token_sizeF;
                            }
                        } else{
                            size_t token1_pos = tmp.find('(', 0);
                            std::string tmpN = "";
                            tmpN = tmpN.assign(tmp, 0, token1_pos);
                            phoneNumber.code_country = std::stoi(tmpN);
                            size_t token2_pos = tmp.find(')', 0);
                            size_t countDel = token2_pos - token1_pos - 1;
                            tmpN = tmpN.assign(tmp, token1_pos + 1, countDel);
                            phoneNumber.code_city = std::stoi(tmpN);
                            int token3_pos = tmp.find(' ', 0);
                            if(token3_pos > 0){
                                tmpN = tmpN.assign(tmp, token2_pos + 1, static_cast<size_t>(token3_pos) - token2_pos);
                                phoneNumber.phone_number = std::stoi(tmpN);
                                tmpN = tmpN.assign(tmp, static_cast<size_t>(token3_pos) + 1, tmp.length() - static_cast<size_t>(token3_pos));
                                phoneNumber.additional_code = std::stoi(tmpN);
                            }else{
                                tmpN = tmpN.assign(tmp, token2_pos + 1, tmp.length());
                                phoneNumber.phone_number = std::stoi(tmpN);
                            }

                        }
                    }
                    pos += tmp.size() + token_size;

                }
                vec.emplace_back(std::make_pair(person,phoneNumber));
                pos = 0;
            }
            }
            file.close();
        }
    friend std::ostream &operator<<(std::ostream &out, const PhoneBook &book);


    void SortByName(){
        std::sort (vec.begin(), vec.end(),
                   [](const auto &left, const auto &right)
                   {return left.first < right.first;});
    }

    void SortByPhone(){
        std::sort(vec.begin(), vec.end(),
                  [](const auto &left, const auto &right)
                  {return left.second < right.second;});
    }


    auto GetPhoneNumber(std::string surname){
        std::tuple<std::string, std::string> tuple("", "");
        int count = 0;

        for(auto &elem : vec){
            if(elem.first.surname == surname) ++count;
        }

        if(count == 0){
            std::get<1>(tuple) = "not found";
            return tuple;
        }else if(count > 1){
            std::get<1>(tuple) = "found more than 1";
            return tuple;
        }

        const std::pair<Person, PhoneNumber> it = *find_if(vec.begin(), vec.end(),
                                                           [surname](const std::pair<Person, PhoneNumber> &p){
            return p.first.surname == surname;
        });

        std::string str = "+";
        str += std::to_string(it.second.code_country);
        str += "(";
        str += std::to_string(it.second.code_city);
        str += ")";
        str += std::to_string(it.second.phone_number);
        str += " ";
        if(it.second.additional_code.has_value())
            str += std::to_string(it.second.additional_code.value());
        std::get<0>(tuple) = str;
        return tuple;
    }

    void ChangePhoneNumber(Person person, PhoneNumber number){

        std::string surname = person.surname;
        std::string name = person.name;
        std::string patronymic = "";
        if(person.patronymic.has_value())
            patronymic = person.patronymic.value();

        const std::pair<Person, PhoneNumber> it = *find_if(vec.begin(), vec.end(), [surname, name, patronymic, number]
        (std::pair<Person, PhoneNumber> &p){
            int check = 0;
            if(p.first.patronymic.has_value()){
                if(p.first.surname == surname && p.first.name == name && p.first.patronymic.value() == patronymic){
                    check = 1;
                }
            }else if(patronymic.empty()){
                if(p.first.surname == surname && p.first.name == name){
                    check = 1;
                }
            }
            if(check == 1){
                p.second.code_country = number.code_country;
                p.second.code_city = number.code_city;
                p.second.phone_number = number.phone_number;
                if(number.additional_code.has_value())
                    p.second.additional_code = number.additional_code.value();
                else p.second.additional_code = std::nullopt;
            }
            return p.first.surname == surname;
        });
    }
};

std::ostream &operator<<(std::ostream &out, const PhoneBook &book){
    for(auto const &element : book.vec){
        out << element.first << " " << element.second << std::endl;
    }
    return out;
}


int main() {

    std::ifstream file("file.txt"); // путь к файлу file.txt
    PhoneBook book(file);
    std::cout << book << std::endl;

    std::cout << "------SortByPhone-------" << std::endl;
    book.SortByPhone();
    std::cout << book << std::endl;

    std::cout << "------SortByName-------" << std::endl;
    book.SortByName();
    std::cout << book << std::endl;

    std::cout << "------GetPhoneNumber-------" << std::endl;
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
    print_phone_number("Egoshin");
    print_phone_number("Donskaya");
    print_phone_number("Trihin");

    std::cout << "------ChangePhoneNumber-------" << std::endl;
    book.ChangePhoneNumber(Person{ "Kashihin", "Antonin", "Kononovich" },
                           PhoneNumber{7, 123, 15344458, std::nullopt});
    book.ChangePhoneNumber(Person{ "Altuhov", "Alfred", "" },
                           PhoneNumber{ 16, 465, 9155448, 13 });
    std::cout << book;

    return 0;
}
