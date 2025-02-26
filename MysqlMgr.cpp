#include "MysqlMgr.h"

MysqlMgr::~MysqlMgr() {
}
int MysqlMgr::RegUser(const std::string& name, const std::string& email, const std::string& pwd)
{
    return m_dao.RegUser(name, email, pwd);
}
MysqlMgr::MysqlMgr() {

}