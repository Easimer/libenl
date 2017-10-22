/*
   Copyright 2017 Daniel Meszaros <easimer@gmail.com>

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include <enl/fs.h>
#include <experimental/filesystem>

using namespace std::experimental;

void fs_create_path(const std::string& path)
{
	filesystem::create_directories(filesystem::path(path));
}

void fs_create_path_to(const std::string& fpath)
{
	auto fpath2 = filesystem::path(fpath);
	if(fpath2.has_parent_path())
		filesystem::create_directories(fpath2.parent_path());
}

std::vector<std::string> fs_list_files(const std::string& path)
{
	std::vector<std::string> ret;
	filesystem::directory_iterator iterator(path);
	
	for(auto& p : iterator)
	{
		ret.push_back(p.path().string());
	}
	
	return ret;
}

std::string fs_concat_paths(const std::string& first, const std::string& second)
{
	filesystem::path ret(first);
	ret += filesystem::path(second);
	
	return ret.string();
}
