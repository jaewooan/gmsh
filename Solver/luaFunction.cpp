#define _FUNCTION_LUA_H
#include "luaFunction.h"
#if defined(HAVE_LUA)
#include <sstream>
#include <string>
#include <vector>
#include "function.h"
#include "Bindings.h"
// function that is defined in Lua

class functionLua::data : public dataCacheDouble{
  private:    
  const functionLua *_function;
  std::vector<dataCacheDouble *> _dependencies;
  public:
  data(const functionLua *f, dataCacheMap *m):
    dataCacheDouble(m->getNbEvaluationPoints(),f->_nbCol),
    _function(f)    
  {
    _dependencies.resize ( _function->_dependenciesName.size());
    for (int i=0;i<_function->_dependenciesName.size();i++)
      _dependencies[i] = &m->get(_function->_dependenciesName[i],this);
  }
  void _eval()
  {
    lua_getfield(_function->_L, LUA_GLOBALSINDEX, _function->_luaFunctionName.c_str());
    for (int i=0;i< _dependencies.size();i++){
      const fullMatrix<double> *data = &(*_dependencies[i])();
      classBinding<const fullMatrix<double> >::push(_function->_L,data,false);
    }
    classBinding<const fullMatrix<double> >::push(_function->_L,&_value,false);
    lua_call(_function->_L,_dependencies.size()+1,0);  /* call Lua function */
  }
};
functionLua::functionLua (int nbCol, std::string &luaFunctionName, std::vector<std::string> &dependencies, lua_State *L)
  : _luaFunctionName(luaFunctionName), _dependenciesName(dependencies),_L(L),_nbCol(nbCol)
{
  static int c=0;
  std::ostringstream oss;
  oss<<"luaFunction_"<<c++;
  _name = oss.str();
  function::add(_name,this);
}
dataCacheDouble *functionLua::newDataCache(dataCacheMap *m)
{
  return new data(this,m);
}

const char *functionLua::className="FunctionLua";
const char *functionLua::parentClassName="Function";
methodBinding *functionLua::methods[]={0};
constructorBinding *functionLua::constructorMethod=new constructorBindingTemplate<functionLua,int,std::string,std::vector<std::string> ,lua_State*>();

#endif // HAVE LUA
