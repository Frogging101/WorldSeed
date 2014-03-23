#include "../globals.hpp"
#include "luabinding.hpp"
#include "GameObjectBinding.hpp"
#include "InputBinding.hpp"
#include "CameraBinding.hpp"
#include "GuiBinding.hpp"
using namespace std;

//TODO make this less ugly, break it up, etc

void bindFunctions(lua_State *l){
	lua_register(l,"print",lua_print);

	//create Gameobject register
	luaL_newmetatable(l, "MetaGO");
	luaL_setfuncs(l, GO_methods, 0);
	lua_pushvalue(l, -1);
	lua_setfield(l, -1,"__index");

	luaL_newlib(l,GO_funcs);
	lua_setglobal(l, "GO");


	//create inputmanager register
	luaL_newlib(l, Input_funcs);
	lua_setglobal(l, "input");

	//add key constants
	lua_newtable(l);
	registerKeys(l);
	lua_setglobal(l,"keys");

	//add mouse conatants
	lua_newtable(l);
	registerMice(l);
	lua_setglobal(l,"mouse");

	//create camera register
	luaL_newmetatable(l, "MetaCam");
	luaL_setfuncs(l, cam_methods, 0);
	lua_pushvalue(l, -1);
	lua_setfield(l, -1,"__index");

	luaL_newlib(l,cam_funcs);
	lua_setglobal(l, "camera");

	//create GUI register
	luaL_newmetatable(l, "MetaGUI");
	luaL_setfuncs(l, GUI_methods, 0);
	lua_pushvalue(l, -1);
	lua_setfield(l, -1,"__index");

	luaL_newlib(l,GUI_funcs);
	lua_setglobal(l, "GUI");
}

float l_toNumber(lua_State *l, int pos){
	if(lua_isnumber(l,pos)){
		return lua_tonumber(l,pos);
	} else {
		lua_pushstring(l, "Argument is not a number");
		lua_error(l);
	}

	return 0;
}

bool l_toBool(lua_State *l, int pos){
	if(lua_isboolean(l,pos)){
		return lua_toboolean(l, pos);
	} else {
		lua_pushstring(l, "Argument is not a boolean");
		lua_error(l);
	}

	return false;
}

string l_toString(lua_State *l, int pos){
	if(lua_isstring(l, pos)){
		return lua_tostring(l, pos);
	} else {
		lua_pushstring(l, "Arugment is not a string");
		lua_error(l);
	}

	return "";
}

int lua_print(lua_State *l){
	string value = "";
	if(lua_isstring(l,1)){
		value = lua_tostring(l,1);
	} else
		value = "nil";
	cout << value << endl;
	global_con->out.print("\n"+value);
	return 0;
}
