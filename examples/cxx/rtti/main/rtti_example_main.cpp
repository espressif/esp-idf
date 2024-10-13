/* C++ run-time type info (RTTI) example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <iostream>
#include <memory>
#include <vector>
#include <algorithm>
#include <cxxabi.h>

using std::cout;
using std::endl;
using std::string;
using std::shared_ptr;
using std::make_shared;

class Base;
class DerivedA;
class DerivedB;

static string demangle(const char* name);

class Base {
public:
    virtual ~Base() {} ;
    virtual string name() = 0;
    static shared_ptr<Base> make_random_derived();
};

class DerivedA : public Base {
public:
    string name() override
    {
        return "DerivedA";
    }
};

class DerivedB : public Base {
public:
    string name() override
    {
        return "DerivedB";
    }
};

/* Creates either DerivedA or DerivedB, depending on a random number */
shared_ptr<Base> Base::make_random_derived()
{
    if (std::rand() % 2 == 0) {
        return make_shared<DerivedA>();
    } else {
        return make_shared<DerivedB>();
    }
}

/* Inside a .cpp file, app_main function must be declared with C linkage */
extern "C" void app_main()
{
    /* Demonstrate typeid().name() */
    cout << "Type names of a few objects:" << endl << '\t';
    cout << "Type name of std::cout is: " << demangle(typeid(std::cout).name()) << endl << '\t';
    cout << "Type name of std::cin is: "  << demangle(typeid(std::cin).name()) << endl << '\t';
    cout << "Type of app_main is: " << demangle(typeid(app_main).name()) << endl << '\t';
    auto sum = [](int x, int y) -> int { return x + y; };
    cout << "Type name of a lambda function is: "  << demangle(typeid(sum).name()) << endl << endl;

    /* Demonstrate dynamic_cast */
    std::vector<shared_ptr<Base>> objects(5);
    cout << "Generating " << objects.size() << " random objects and printing their types:" << endl;
    std::generate(objects.begin(), objects.end(), Base::make_random_derived);
    for (auto &obj : objects) {
        cout << "obj->name() is: " << obj->name() << endl << '\t';
        cout << "typeid(*obj).name() is: " << demangle(typeid(*obj).name()) << endl << '\t';

        const DerivedA* cast_to_derived_a = dynamic_cast<DerivedA*>(obj.get());
        const DerivedB* cast_to_derived_b = dynamic_cast<DerivedB*>(obj.get());

        cout << "dynamic_cast<DerivedA*>(obj)=" << static_cast<const void*>(cast_to_derived_a) << endl << '\t';
        cout << "dynamic_cast<DerivedB*>(obj)=" << static_cast<const void*>(cast_to_derived_b) << endl << endl;
    }

    cout << "Example finished." << endl;
}

/* Helper function which converts typeid().name() to a human-readable type name */
static std::string demangle(const char* name)
{
    int status = 0;
    char* result = abi::__cxa_demangle(name, NULL, NULL, &status);
    string str_result;
    if (status == 0) {
        str_result = result;
    } else {
        str_result = name;
    }
    free(result);
    return str_result;
}
