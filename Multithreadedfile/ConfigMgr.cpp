#include "ConfigMgr.h"

std::string ConfigMgr::GetValue(const std::string& section, const std::string& key)
{
	auto it = m_config_map.find(section);
	return (it != m_config_map.end() ? it->second.GetValue(key) : "");
}

bfs::path ConfigMgr::GetFileOutPath()
{
	return m_static_path;
}

void ConfigMgr::InitPath()
{
	namespace fs = boost::filesystem;

	const fs::path current_path = fs::current_path();
	const std::string binder = GetValue("Output", "Path");
	const std::string staticdir = GetValue("Static", "Path");

	m_static_path = current_path / binder / staticdir;
	m_bin_path = current_path / binder;

	// 检查路径是否存在
	if (!boost::filesystem::exists(m_static_path)) {
		// 如果路径不存在，创建它
		if (boost::filesystem::create_directories(m_static_path)) {
			std::cout << "路径已成功创建: " << m_static_path.string() << std::endl;
		}
		else {
			std::cerr << "创建路径失败: " << m_static_path.string() << std::endl;
		}
	}
	else {
		std::cout << "路径已存在: " << m_static_path.string() << std::endl;
	}
}

ConfigMgr::ConfigMgr()
{
	//获取当前工作目录
	boost::filesystem::path current_path = boost::filesystem::current_path();
	// 构建配置文件完整路径（当前路径/config.ini）
	boost::filesystem::path config_path = current_path / "config.ini";
	std::cout << "[Config] Loading config from: " << config_path << std::endl;

	// 使用Boost解析INI文件
	boost::property_tree::ptree pt;
	try
	{
		boost::property_tree::read_ini(config_path.string(), pt);
	}
	catch (const boost::property_tree::ini_parser_error& e)
	{
		std::cerr << "[Config] Error reading config file: " << e.what() << std::endl;
		return;
	}

	//遍历所有配置节(section)
	for (const auto& section_pair : pt)
	{
		const std::string& section_name = section_pair.first;
		const auto& section_tree = section_pair.second;
		// 对于每个section，遍历其所有的key-value对  
		std::map<std::string, std::string> section_config;
		for (const auto& key_value_pair : section_tree)
		{
			const std::string& key = key_value_pair.first;
			const std::string value = key_value_pair.second.get_value<std::string>();
			section_config.emplace(key, value);
		}

		SectionInfo sectionInfo;
		sectionInfo.m_section_datas = std::move(section_config);
		m_config_map.emplace(section_name, std::move(sectionInfo));
	}
	// 输出所有的section和key-value对  
	for (const auto& section_entry : m_config_map) {
		const std::string& section_name = section_entry.first;
		SectionInfo section_config = section_entry.second;
		std::cout << "[" << section_name << "]" << std::endl;
		for (const auto& key_value_pair : section_config.m_section_datas) {
			std::cout << key_value_pair.first << "=" << key_value_pair.second << std::endl;
		}
	}

	//初始化配置
	InitPath();
}

