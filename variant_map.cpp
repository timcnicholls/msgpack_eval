#include <string>
#include <sstream>
#include <iostream>

#include <msgpack.hpp>

class User
{
public:

    User() : name("unknown"), id(0) {}

    User(std::string name, int id=0) : name(name), id(id) {}

    void set_name(std::string const& name)
    {
        this->name = name;
    }

    void set_id(int id)
    {
        this->id = id;
    }

    void set_param(std::string const& name, msgpack::type::variant const& id)
    {
        this->params.insert(std::pair<std::string, msgpack::type::variant>(name, id));
    }

    template <typename T> T get_param(std::string const& name)
    {
        std::map<std::string, msgpack::type::variant>::iterator it = this->params.find(name);
        if (it != this->params.end()) {
            return get_param_value<T>(it->second);
        }
        else
        {
            return 0;
        }
    }

    template <typename T> T get_param_value(msgpack::type::variant const& value);

    friend bool operator==(User const& lhs, User const& rhs) {
        bool equal = true;
        equal &= (lhs.name == rhs.name);
        equal &= (lhs.id == rhs.id);
        equal &= (lhs.params == rhs.params);
        return equal;
    }

    MSGPACK_DEFINE_MAP(name, id, params);

private:
    std::string name;
    int id;
    std::map<std::string, msgpack::type::variant> params;

};

template <> int User::get_param_value(msgpack::type::variant const& value)
{
    if (value.is_uint64_t())
    {
        return static_cast<int>(value.as_uint64_t());
    }
    else if (value.is_int64_t())
    {
        return static_cast<int>(value.as_int64_t());
    }
    else
    {
        return 0;
        //TODO exception here
    }
}

template <> std::string User::get_param_value(msgpack::type::variant const& value)
{
    return value.as_string();
}

template <> double User::get_param_value(msgpack::type::variant const& value)
{
    return value.as_double();
}

void print(std::string const& buf) {
    for (std::string::const_iterator it = buf.begin(), end = buf.end();
    it != end;
    ++it) {
        std::cout
            << std::setw(2)
            << std::hex
            << std::setfill('0')
            << (static_cast<int>(*it) & 0xff)
            << " ";
    }
    std::cout << std::dec << std::endl;
}

void print_as_python_bytes(std::string const& buf)
{
    std::cout << "b\'";
    for (std::string::const_iterator it = buf.begin(), end = buf.end(); it!= end; ++it)
    {
        std::cout
            << "\\x"
            << std::setw(2)
            << std::hex
            << std::setfill('0')
            << (static_cast<int>(*it) & 0xff);
    }
    std::cout << std::dec << "\'" << std::endl;
}

int main(void)
{

    User u("Tim Nicholls", 1234);
    u.set_param("paramOne",4567);
    u.set_param("paramTwo", "two");
    u.set_param("paramThree", 3.14159265359);


    std::stringstream ss;
    msgpack::pack(ss, u);

    print(ss.str());
    print_as_python_bytes(ss.str());

    msgpack::unpacked unp;
    msgpack::unpack(unp, ss.str().data(), ss.str().size());
    msgpack::object obj = unp.get();
    std::cout << obj << std::endl;

    User v = obj.as<User>();
    std::cout << "paramOne has value " << v.get_param<int>("paramOne") << std::endl;
    std::cout << "paramTwo has value " << v.get_param<std::string>("paramTwo") << std::endl;
    std::cout << "paramThree has value " << v.get_param<double>("paramThree") << std::endl;

    assert(v == u);

    return 0;
}
