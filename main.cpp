#include <iostream>
#include <string>
#include <optional>
#include <tuple>
#include <vector>
#include <fstream>
#include <algorithm>

struct Person{
    std::string name;
    std::string surname;
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
            auto count = 0;

            Person person;
            PhoneNumber phoneNumber;

            while (getline(file,s)){
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
                                tmpF = "";
                                tmpF = tmpF.assign(tmp, posF, tmp.find(tokenF, posF) - posF);
                                ++count;
                                if(count == 1){
                                    std::cout << count << "_";
                                    person.surname = tmpF;
                                    std::cout << person.surname << " ";
                                }else if(count == 2){
                                    std::cout << count << "_";
                                    person.name = tmpF;
                                    std::cout << person.name << " ";
                                }else if(count == 3){
                                    std::cout << count << "_";
                                    person.patronymic = tmpF;
                                    std::cout << person.patronymic.value() << " ";
                                }
                                posF += tmpF.size() + token_sizeF;
                            }
                            std::cout << std::endl;
                        } else{
                            size_t token1_pos = tmp.find("(", 0);
                            std::string tmpN = "";
                            tmpN = tmpN.assign(tmp, 0, token1_pos);
                            phoneNumber.code_country = std::stoi(tmpN);
                            size_t token2_pos = tmp.find(")", 0);
                            size_t countDel = token2_pos - token1_pos - 1;
                            tmpN = tmpN.assign(tmp, token1_pos + 1, countDel);
                            phoneNumber.code_city = std::stoi(tmpN);
                            int token3_pos = tmp.find(" ", 0);
                            if(token3_pos > 0){
                                tmpN = tmpN.assign(tmp, token2_pos + 1, static_cast<size_t>(token3_pos) - token2_pos);
                                phoneNumber.phone_number = std::stoi(tmpN);
                                tmpN = "";
                                tmpN = tmpN.assign(tmp, static_cast<size_t>(token3_pos) + 1, tmp.length() - static_cast<size_t>(token3_pos));
                                phoneNumber.additional_code = std::stoi(tmpN);
                            }else{
                                tmpN = "";
                                tmpN = tmpN.assign(tmp, token2_pos + 1, tmp.length());
                                phoneNumber.phone_number = std::stoi(tmpN);
                            }

                        }
                    }
                    pos += tmp.size() + token_size;

                }
                vec.push_back(std::make_pair(person,phoneNumber));
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



    };

std::ostream &operator<<(std::ostream &out, const PhoneBook &book){
    for(auto const &element : book.vec){
        out << element.first << " " << element.second << std::endl;
    }
    return out;
}



int main() {

    std::ifstream file("file.txt");
    PhoneBook book(file);
    std::cout << book << std::endl;
    book.SortByName();
    //std::cout << book << std::endl;
    book.SortByPhone();
    //std::cout << book << std::endl;



    return 0;
}
