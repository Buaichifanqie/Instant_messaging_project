#pragma once
#include "const.h"
struct SectionInfo {
    // 默认构造函数（可以省略）
    SectionInfo() {}

    // 析构函数（可以省略）
    ~SectionInfo()
    {
        m_section_datas.clear();
    }

    // 拷贝构造函数
    SectionInfo(const SectionInfo& src) {
        m_section_datas = src.m_section_datas;
    }

    // 拷贝赋值运算符
    SectionInfo& operator=(const SectionInfo& src) {
        if (&src == this) {
            return *this;
        }
        m_section_datas = src.m_section_datas;
        return *this;
    }

    // 重载 [] 运算符
    std::string operator[](const std::string& key) const {
        auto it = m_section_datas.find(key);
        if (it == m_section_datas.end()) {
            return "";
        }
        return it->second;
    }

    // 成员变量
    std::map<std::string, std::string> m_section_datas;
};

class ConfigMgr
{
public:
    ~ConfigMgr() {
        m_config_map.clear();
    }

    SectionInfo operator[](const std::string& section) {
        if (m_config_map.find(section) == m_config_map.end()) {
            return SectionInfo();
        }
        return m_config_map[section];
    }

    static ConfigMgr& Inst()
    {
        static ConfigMgr cfg_mgr;
        return cfg_mgr;
    }

    ConfigMgr& operator=(const ConfigMgr& src) {
        if (&src == this) {
            return *this;
        }
        this->m_config_map = src.m_config_map;
    };
    ConfigMgr(const ConfigMgr& src) {
        this->m_config_map = src.m_config_map;
    }
         

private:
    ConfigMgr();
    std::map<std::string, SectionInfo> m_config_map;
};

