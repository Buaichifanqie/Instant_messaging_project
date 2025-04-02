#pragma once
#include <fstream>  
#include <boost/property_tree/ptree.hpp>  
#include <boost/property_tree/ini_parser.hpp>  
#include <boost/filesystem.hpp>    
#include <map>
#include <iostream>

//前置声明
namespace bfs = boost::filesystem;

//定义一个结构体存储配置节数据的结构体，封装节内键值对的操作
struct SectionInfo
{
	SectionInfo() = default;
	~SectionInfo()
	{
		m_section_datas.clear();
	}

	SectionInfo(const SectionInfo& src)
	{
		m_section_datas = src.m_section_datas;
	}

	SectionInfo& operator=(const SectionInfo& src)
	{
		if (&src == this)
		{
			return *this;
		}
		this->m_section_datas = src.m_section_datas;
		return *this;
	}
	std::string  operator[](const std::string& key) 
	{
		auto it = m_section_datas.find(key);
		return (it != m_section_datas.end()) ? it->second : "";
	}

	std::string GetValue(const std::string& key)
	{
		return operator[](key);
	}

	std::map<std::string, std::string> m_section_datas;
};

// 配置管理单例类，提供配置访问和路径管理功能
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


	ConfigMgr& operator=(const ConfigMgr& src) {
		if (&src == this) {
			return *this;
		}

		this->m_config_map = src.m_config_map;
	};

	ConfigMgr(const ConfigMgr& src) {
		this->m_config_map = src.m_config_map;
	}

	static ConfigMgr& Inst() {
		static ConfigMgr cfg_mgr;
		return cfg_mgr;
	}

	std::string GetValue(const std::string& section, const std::string& key);
	boost::filesystem::path GetFileOutPath();
	void InitPath();

private:
	ConfigMgr();
	std::map<std::string, SectionInfo> m_config_map;
	bfs::path m_static_path;
	bfs::path m_bin_path;
};

