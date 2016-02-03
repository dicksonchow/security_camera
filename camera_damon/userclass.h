#include <string>
using namespace std;

class User
{
	private:
		int id;
		string name;
		bool isRegister;

	public:
		User(int id, string name, bool isRegister): id(id), name(name), isRegister(isRegister) { }
		string get_user_name() { return name; }
		bool get_register_status() { return isRegister; }
};
