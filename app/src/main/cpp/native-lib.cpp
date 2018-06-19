#include <jni.h>
#include <string>


#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
#include <string>

using namespace std;
#define to_string(s) # s


extern "C"
JNIEXPORT jstring
JNICALL
Java_jbox2d_example_com_ffmpeg_1demo_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";

//    // vector
//    vector<int> v1;
//    vector<int> v2;
//    v1.push_back(0);
//
//    v1.at(5);
//
//    v1.back();
//    v1.clear();
//    if (v1.empty()) {
//        v1.erase(5);
//        v1.pop_back();
//    }
//
//    v1.operator=(v2);
//
//
//    // 便利
//    vector<int> v3;
//    vector<int>::iterator it;
//    for (it = v3.begin(); it != v3.end(); it++) {
//        (*it);
////        v3.erase(it); // 删除制定位置
//    }
//
//    vector<int> v4;
//    for (int i = 0; i < 5; ++i) {
//        v4.push_back(i);
//    }
//    for (int i = 0; i < 5; ++i) {
//        cout << v4.back() << end;
//
//
//        v4.pop_back();
//    }
    return env->NewStringUTF(hello.c_str());
}
