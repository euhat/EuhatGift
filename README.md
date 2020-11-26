# EuhatExpert
This is an open source code of Euhat Gift project.

## Official site
[http://euhat.com/wp/category/%e8%bd%af%e4%bb%b6/](http://euhat.com/wp/category/%e8%bd%af%e4%bb%b6/) 

## It has features
* A c++ wapper class for cJSON library.
* Ini file read & write.

## Why create this utilities
1. CjsonWrapper, Boost library for ini file read & write is very convenient for writing business code, but which treats integer as number string is problematic. And consideration on efficiency and multi-language friendliness, I write CjsonWrapper, a c++ wapper class for cJSON library.
2. IniOp, Win32 GetPrivateProfileStringA function has a defect that we must allocate a buffer in advance, but we don't know what size of the actual value buffer. So I rewrite ini file operation functions, namely IniOp, which is cross-platform also.

## How to build
1. In Linux OS, change directory to EuhatGift/test/, type make to compile the test code, then you can run tstCjsonWrapper and tstIniOp to know how they work.

## About 3rd open sources
1. cJSON, copyright belongs to Dave Gamble

## FAQ

Have any question, welcome to contact me through email: euhat@hotmail.com
