#include <jni.h>
#include <string>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <android/log.h>
#include <sys/ptrace.h>
#include <unistd.h>

#define TAG_SHIELD "SHIELD_CORE"
#define LOG_I(...) __android_log_print(ANDROID_LOG_INFO,  TAG_SHIELD, __VA_ARGS__)
#define LOG_W(...) __android_log_print(ANDROID_LOG_WARN,  TAG_SHIELD, __VA_ARGS__)
#define LOG_E(...) __android_log_print(ANDROID_LOG_ERROR, TAG_SHIELD, __VA_ARGS__)

static bool probeTracer() {
    long r = ptrace(PTRACE_TRACEME, 0, nullptr, nullptr);
    if (r == -1) {
        LOG_E("[SONDE-1] Processus deja trace -- supervision externe probable");
        return true;
    }
    LOG_I("[SONDE-1] Aucune supervision ptrace detectee");
    return false;
}

static bool probeMemoryMap() {
    FILE* procMaps = fopen("/proc/self/maps", "r");
    if (!procMaps) {
        LOG_W("[SONDE-2] Lecture de /proc/self/maps impossible");
        return false;
    }

    const char* signatures[] = {
        "frida", "libfrida", "xposed",
        "gdbserver", "libgdb", "magisk",
        "substrate", "zygisk"
    };
    const int sigCount = 8;

    char row[512];
    while (fgets(row, sizeof(row), procMaps)) {
        for (int i = 0; i < sigCount; i++) {
            if (strstr(row, signatures[i])) {
                LOG_E("[SONDE-2] Entree suspecte dans maps => %s", row);
                fclose(procMaps);
                return true;
            }
        }
    }

    fclose(procMaps);
    LOG_I("[SONDE-2] Aucune entree suspecte dans /proc/self/maps");
    return false;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_example_lab23_1jni_HomeActivity_runSecurityProbes(
        JNIEnv* env,
        jobject thiz) {

    bool t = probeTracer();
    bool m = probeMemoryMap();

    if (t && m)  { LOG_E("[SHIELD] Vecteur combine detecte"); return 3; }
    if (t)       { LOG_E("[SHIELD] Vecteur tracer seul");     return 1; }
    if (m)       { LOG_E("[SHIELD] Vecteur maps seul");       return 2; }

    LOG_I("[SHIELD] Environnement sain");
    return 0;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_lab23_1jni_HomeActivity_nativeGreeting(
        JNIEnv* env,
        jobject thiz) {
    return env->NewStringUTF("Bonjour depuis le moteur C++ natif !");
}

extern "C"
JNIEXPORT jlong JNICALL
Java_com_example_lab23_1jni_HomeActivity_computeFactorial(
        JNIEnv* env,
        jobject thiz,
        jint value) {

    if (value < 0) return -1L;
    long long acc = 1;
    for (int k = 2; k <= value; k++) acc *= k;
    return static_cast<jlong>(acc);
}