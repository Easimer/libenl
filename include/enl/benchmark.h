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

#pragma once

#include <iostream>
#include <chrono>

class benchmark {
public:

	benchmark() {
		m_name = "";
	}

	benchmark(std::string& name) {
		m_name = name;
	}

	void start() {
		m_start = std::chrono::high_resolution_clock::now();
		m_ended = false;
	}

	double end() {
		m_end = std::chrono::high_resolution_clock::now();
		m_elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(m_end - m_start).count();
		m_ended = true;
		return m_elapsed;
	}

	double elapsed() {
		return m_elapsed;
	}

	void result() {
		if(m_ended) {
			std::cout << "Benchmark " << m_name << ": " << m_elapsed << " seconds." << std::endl;
		} else {
			std::cout << "Benchmark is still running" << std::endl;
		}
	}

private:
	std::string m_name;
	std::chrono::high_resolution_clock::time_point m_start, m_end;
	double m_elapsed;
	bool m_ended = false;
};

