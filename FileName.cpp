
//学生
//王 123456 123456
//老师
//杨 123456 123456
//管理员
//123456 123456
#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include <windows.h>
#include <conio.h>
#include <ctype.h>   // isdigit()需要
#include<string>
#include<time.h>

using namespace std;

#define MAX_STUDENTS 1000   // 最大学生数量
#define MAX_COURSES 8       // 最大课程数量
#define MAX_NAME_LEN 50     // 姓名最大长度
#define MAX_ID_LEN 10       // 学号最大长度
#define MAX_CLASS_LEN 30    // 班级最大长度
#define PASSWORD_LEN 10         // 密码长度
#define PWD_MASK '*'
#define STUDENT_FILE "students.bin"
#define TEACHER_FILE "teachers.bin"
#define ADMIN_FILE "admins.bin"
#define PWD_REQUEST_FILE "pwd_requests.bin"  // 密码找回申请文件
char loginTeacherId[MAX_ID_LEN] = { 0 };    // 当前登录教师的工号
char loginTeacherClass[MAX_CLASS_LEN] = { 0 }; // 当前登录教师管理的班级（修复报错的变量）
void loadStudentsToLinkList();

//////////////////////////////////////////////////////////////////////////////////////
//隐藏式输入密码，输入结果存入pwd，max_len为密码最大长度
void inputPassword(char* pwd, int max_len) {
    int i = 0;
    char ch;
    memset(pwd, 0, max_len);  // 清空密码数组
    printf("请输入密码：");
    while (1) {
        ch = _getch();  // 无回显获取按键
        if (ch == '\r' || ch == '\n') {  // 按回车结束输入
            break;
        }
        else if (ch == '\b' && i > 0) {  // 按退格键删除
            printf("\b \b");  // 删掉屏幕上的*
            i--;
        }
        else if (i < max_len - 1) {  // 正常输入字符
            pwd[i++] = ch;
            printf("%c", PWD_MASK);    // 显示*
        }
    }
    printf("\n");
}

// 清空输入缓冲区（你原有代码中缺失，需补充）
void clearInputBuffer() {
    while (getchar() != '\n'); // 读取并丢弃缓冲区所有字符，直到回车
}
// 判断字符串是否为数字（用于学号/成绩验证）
int isNumber(char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit(str[i]))
            return 0;
    }
    return 1;
}
//////////////////////////////////////////////////////////////////////////////////////////

typedef struct Student {
    char id[MAX_ID_LEN];           //学号
    char password[PASSWORD_LEN];   //密码
    char name[MAX_NAME_LEN];       //姓名
    char classname[MAX_CLASS_LEN]; //班级
    int age;                       //年龄
    char sex;                   //性别(M/F)
    int chinese;                //语文成绩
    int math;                   //数学成绩
    int english;                //英语成绩
    int physics;                //物理成绩
    int chemistry;              //化学成绩
    int biology;                //生物成绩
    float total_score;           //总分
    int rank;                    //排名
    struct Student* next;
}Student;
Student loginStudent;

// 密码找回申请结构体
typedef struct PwdRequest {
    char id[MAX_ID_LEN];           // 申请账号（学号/工号）
    char name[MAX_NAME_LEN];       // 姓名
    char classname[MAX_CLASS_LEN]; // 班级（学生用）
    int role;                      // 1-学生, 2-教师
    int status;                    // 0-待审批, 1-已批准, 2-已拒绝
    time_t requestTime;            // 申请时间
} PwdRequest;

// 计算总分
void calculateTotalScore(Student* stu) {
    stu->total_score = (float)(stu->chinese + stu->math + stu->english +
        stu->physics + stu->chemistry + stu->biology);
}

// 学生登录验证：返回1表示登录成功，0失败，-1文件错误
int studentLogin(char* inputId, char* inputPwd) {
    FILE* fp = fopen(STUDENT_FILE, "rb");
    if (fp == NULL) {
        printf("账号文件不存在或打开失败！\n");
        return -1;
    }
    Student stu;
    // 循环读取文件中的每个学生信息
    while (fread(&stu, sizeof(Student), 1, fp) == 1) {
        // 比对学号和密码
        if (strcmp(stu.id, inputId) == 0 && strcmp(stu.password, inputPwd) == 0) {
            // 新增：将登录学生信息赋值给全局变量
            memcpy(&loginStudent, &stu, sizeof(Student));
            // 新增：加载学生链表（确保成绩数据可用）
            loadStudentsToLinkList();
            fclose(fp);
            printf("登录成功！欢迎你，%s同学\n", stu.name);
            return 1;
        }
    }
    fclose(fp);
    printf("账号或密码错误！\n");
    return 0;
}

// 学生账号注册：返回1成功，0失败（账号已存在），-1文件错误
int studentRegister() {
    Student newStu;
    FILE* fp = fopen(STUDENT_FILE, "ab+");  // 追加+二进制模式
    if (fp == NULL) {
        printf("文件打开失败！\n");
        return -1;
    }
    // 1. 输入账号（学号）并检查是否已存在
    printf("请输入学号（账号）：");
    scanf("%s", newStu.id);
    // 先检查账号是否重复
    Student temp;
    fseek(fp, 0, SEEK_SET);  // 回到文件开头
    while (fread(&temp, sizeof(Student), 1, fp) == 1) {
        if (strcmp(temp.id, newStu.id) == 0) {
            fclose(fp);
            printf("该学号已注册！\n");
            return 0;
        }
    }
    // 2. 输入其他信息
    printf("请输入姓名：");
    scanf("%s", newStu.name);
    printf("请输入班级（如：高一(1)班）：");
    scanf("%s", newStu.classname);
    printf("请输入年龄：");
    scanf("%d", &newStu.age);
    printf("请输入性别（M/F）：");
    scanf(" %c", &newStu.sex);
    newStu.chinese = 0;
    newStu.math = 0;
    newStu.english = 0;
    newStu.physics = 0;
    newStu.chemistry = 0;
    newStu.biology = 0;
    // 计算总分（初始为0）
    calculateTotalScore(&newStu);
    newStu.rank = 0;
    newStu.next = NULL;
    // 3. 隐藏输入密码
    char pwd1[20], pwd2[20];
    inputPassword(pwd1, 20);
    printf("请再次输入密码：");
    inputPassword(pwd2, 20);
    if (strcmp(pwd1, pwd2) != 0) {
        fclose(fp);
        printf("两次密码输入不一致！\n");
        return 0;
    }
    strcpy(newStu.password, pwd1);

    // 4. 写入文件
    fwrite(&newStu, sizeof(Student), 1, fp);
    fclose(fp);
    printf("注册成功！\n");
    return 1;
}

// 修改学生密码：输入原账号和原密码，验证通过后修改
int changeStudentPwd(char* inputId, char* oldPwd) {
    FILE* fp;
    errno_t err = fopen_s(&fp, STUDENT_FILE, "rb+");
    if (err != 0 || fp == NULL) {
        printf("文件打开失败！\n");
        return -1;
    }

    Student stu;
    int found = 0;
    while (fread(&stu, sizeof(Student), 1, fp) == 1) {
        if (strcmp(stu.id, inputId) == 0 && strcmp(stu.password, oldPwd) == 0) {
            found = 1;
            char newPwd1[20], newPwd2[20];
            printf("请输入新密码：");
            inputPassword(newPwd1, 20);
            printf("请再次输入新密码：");
            inputPassword(newPwd2, 20);
            if (strcmp(newPwd1, newPwd2) != 0) {
                fclose(fp);
                printf("两次密码不一致！\n");
                return 0;
            }
            fseek(fp, -(long)sizeof(Student), SEEK_CUR);
            strcpy(stu.password, newPwd1);
            fwrite(&stu, sizeof(Student), 1, fp);
            break;
        }
    }
    fclose(fp);
    if (!found) {
        printf("账号或原密码错误！\n");
        return 0;
    }
    printf("密码修改成功！\n");
    return 1;
}

Student* stuListHead = NULL;    // 学生链表头节点

//查询个人成绩
void queryMyScore() {
    Student* temp = stuListHead;
    while (temp != NULL) {
        if (strcmp(temp->id, loginStudent.id) == 0) {
            printf("\n================== 个人成绩详情 =================\n");
            printf("学号：%s\t姓名：%s\t班级：%s\n", temp->id, temp->name, temp->classname);
            printf("年龄：%d\t性别：%c\n", temp->age, temp->sex);
            printf("-------------------------------------------------\n");
            printf("语文：%d\t数学：%d\t英语：%d\n", temp->chinese, temp->math, temp->english);
            printf("物理：%d\t化学：%d\t生物：%d\n", temp->physics, temp->chemistry, temp->biology);
            printf("-------------------------------------------------\n");
            printf("总分：%.1f\t班级排名：第%d名\n", temp->total_score, temp->rank);
            return;
        }
        temp = temp->next;
    }
    printf("暂无你的成绩数据！\n");
}

// 按班级分组计算排名（每个班级独立排名）
void sortStudentByTotalScoreByClass() {
    if (stuListHead == NULL) return;
    // 1. 先按班级分组，存储每个班级的学生链表
    char currentClass[MAX_CLASS_LEN];
    Student* classHead = NULL; // 临时存储当前班级的学生
    Student* temp = stuListHead;
    while (temp != NULL) {
        strcpy(currentClass, temp->classname);
        // 提取当前班级的所有学生到classHead
        classHead = NULL;
        Student* classTail = NULL;
        Student* p = stuListHead;
        while (p != NULL) {
            if (strcmp(p->classname, currentClass) == 0) {
                Student* newNode = (Student*)malloc(sizeof(Student));
                memcpy(newNode, p, sizeof(Student));
                newNode->next = NULL;
                if (classHead == NULL) {
                    classHead = newNode;
                    classTail = newNode;
                }
                else {
                    classTail->next = newNode;
                    classTail = newNode;
                }
            }
            p = p->next;
        }

        // 2. 对当前班级的学生排序（降序）
        if (classHead != NULL && classHead->next != NULL) {
            int swapped;
            Student* a;
            Student* b = NULL;
            do {
                swapped = 0;
                a = classHead;
                while (a->next != b) {
                    if (a->total_score < a->next->total_score) {
                        // 交换两个节点的数据
                        char tempId[MAX_ID_LEN], tempPwd[PASSWORD_LEN], tempName[MAX_NAME_LEN], tempClass[MAX_CLASS_LEN];
                        int tempAge;
                        char tempSex;
                        int tempChinese, tempMath, tempEnglish, tempPhysics, tempChemistry, tempBiology;
                        float tempTotal;

                        strcpy(tempId, a->id); strcpy(a->id, a->next->id); strcpy(a->next->id, tempId);
                        strcpy(tempPwd, a->password); strcpy(a->password, a->next->password); strcpy(a->next->password, tempPwd);
                        strcpy(tempName, a->name); strcpy(a->name, a->next->name); strcpy(a->next->name, tempName);
                        strcpy(tempClass, a->classname); strcpy(a->classname, a->next->classname); strcpy(a->next->classname, tempClass);
                        tempAge = a->age; a->age = a->next->age; a->next->age = tempAge;
                        tempSex = a->sex; a->sex = a->next->sex; a->next->sex = tempSex;

                        tempChinese = a->chinese; a->chinese = a->next->chinese; a->next->chinese = tempChinese;
                        tempMath = a->math; a->math = a->next->math; a->next->math = tempMath;
                        tempEnglish = a->english; a->english = a->next->english; a->next->english = tempEnglish;
                        tempPhysics = a->physics; a->physics = a->next->physics; a->next->physics = tempPhysics;
                        tempChemistry = a->chemistry; a->chemistry = a->next->chemistry; a->next->chemistry = tempChemistry;
                        tempBiology = a->biology; a->biology = a->next->biology; a->next->biology = tempBiology;
                        tempTotal = a->total_score; a->total_score = a->next->total_score; a->next->total_score = tempTotal;

                        swapped = 1;
                    }
                    a = a->next;
                }
                b = a;
            } while (swapped);

            // 3. 更新当前班级学生的排名（仅在本班内排名）
            int rank = 1;
            Student* classTemp = classHead;
            while (classTemp != NULL) {
                // 找到原链表中对应的学生，更新排名
                Student* origin = stuListHead;
                while (origin != NULL) {
                    if (strcmp(origin->id, classTemp->id) == 0) {
                        origin->rank = rank;
                        break;
                    }
                    origin = origin->next;
                }
                rank++;
                classTemp = classTemp->next;
            }

            // 释放临时班级链表内存
            classTemp = classHead;
            while (classTemp != NULL) {
                Student* next = classTemp->next;
                free(classTemp);
                classTemp = next;
            }
        }

        // 跳到下一个不同的班级（避免重复处理）
        while (temp != NULL && strcmp(temp->classname, currentClass) == 0) {
            temp = temp->next;
        }
    }
}

// 查询本班成绩
void queryClassScore(const char* className) {
    if (stuListHead == NULL) {
        printf("暂无学生成绩数据！\n");
        return;
    }
    sortStudentByTotalScoreByClass();
    printf("\n========================= 本班（%s）成绩排名 ========================\n", className);
    printf("排名\t学号\t姓名\t语文\t数学\t英语\t物理\t化学\t生物\t总分\n");
    printf("==============================================================================\n");

    Student* temp = stuListHead;
    int hasClassData = 0;
    while (temp != NULL) {
        if (strcmp(temp->classname, className) == 0) {
            hasClassData = 1;
            printf("%d\t%s\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t%.1f\n",
                temp->rank, temp->id, temp->name,
                temp->chinese, temp->math, temp->english,
                temp->physics, temp->chemistry, temp->biology,
                temp->total_score);
        }
        temp = temp->next;
    }
    if (!hasClassData) {
        printf("本班暂无成绩数据！\n");
    }
}

// 成绩分析
void analyzeMyScore() {
    Student* myScore = NULL;
    Student* temp = stuListHead;
    while (temp != NULL) {
        if (strcmp(temp->id, loginStudent.id) == 0) {
            myScore = temp;
            break;
        }
        temp = temp->next;
    }
    if (myScore == NULL) {
        printf("暂无你的成绩数据，无法分析！\n");
        return;
    }
    // 统计本班数据
    temp = stuListHead;
    int classCount = 0;
    int classChinese = 0, classMath = 0, classEnglish = 0;
    int classPhysics = 0, classChemistry = 0, classBiology = 0;
    float classTotal = 0;
    while (temp != NULL) {
        if (strcmp(temp->classname, loginStudent.classname) == 0) {
            classCount++;
            classChinese += temp->chinese;
            classMath += temp->math;
            classEnglish += temp->english;
            classPhysics += temp->physics;
            classChemistry += temp->chemistry;
            classBiology += temp->biology;
            classTotal += temp->total_score;
        }
        temp = temp->next;
    }
    if (classCount == 0) {
        printf("本班暂无成绩数据，无法分析！\n");
        return;
    }
    // 计算平均分
    float avgChinese = (float)classChinese / classCount;
    float avgMath = (float)classMath / classCount;
    float avgEnglish = (float)classEnglish / classCount;
    float avgPhysics = (float)classPhysics / classCount;
    float avgChemistry = (float)classChemistry / classCount;
    float avgBiology = (float)classBiology / classCount;
    float avgTotal = classTotal / classCount;
    // 展示分析报告
    printf("\n====================== 成绩分析报告 ======================\n");
    printf("1. 基础信息：\n");
    printf("   姓名：%s\t班级：%s\t本班总人数：%d\n", myScore->name, myScore->classname, classCount);
    printf("   总分排名：第%d名\t总分：%.1f\t班级平均分：%.1f\n", myScore->rank, myScore->total_score, avgTotal);
    printf("\n2. 各科成绩与班级平均分对比：\n");
    printf("   科目\t你的成绩\t班级平均分\t差距（高/低）\n");
    printf("   语文\t%d\t\t%.1f\t\t%.1f\n", myScore->chinese, avgChinese, myScore->chinese - avgChinese);
    printf("   数学\t%d\t\t%.1f\t\t%.1f\n", myScore->math, avgMath, myScore->math - avgMath);
    printf("   英语\t%d\t\t%.1f\t\t%.1f\n", myScore->english, avgEnglish, myScore->english - avgEnglish);
    printf("   物理\t%d\t\t%.1f\t\t%.1f\n", myScore->physics, avgPhysics, myScore->physics - avgPhysics);
    printf("   化学\t%d\t\t%.1f\t\t%.1f\n", myScore->chemistry, avgChemistry, myScore->chemistry - avgChemistry);
    printf("   生物\t%d\t\t%.1f\t\t%.1f\n", myScore->biology, avgBiology, myScore->biology - avgBiology);
    // 薄弱科目
    printf("\n3. 薄弱科目建议（低于班级平均分）：\n");
    int hasWeak = 0;
    if (myScore->chinese < avgChinese) {
        printf("   语文（低于平均分%.1f分）\n", avgChinese - myScore->chinese);
        hasWeak = 1;
    }
    if (myScore->math < avgMath) {
        printf("   数学（低于平均分%.1f分）\n", avgMath - myScore->math);
        hasWeak = 1;
    }
    if (myScore->english < avgEnglish) {
        printf("   英语（低于平均分%.1f分）\n", avgEnglish - myScore->english);
        hasWeak = 1;
    }
    if (myScore->physics < avgPhysics) {
        printf("   物理（低于平均分%.1f分）\n", avgPhysics - myScore->physics);
        hasWeak = 1;
    }
    if (myScore->chemistry < avgChemistry) {
        printf("   化学（低于平均分%.1f分）\n", avgChemistry - myScore->chemistry);
        hasWeak = 1;
    }
    if (myScore->biology < avgBiology) {
        printf("   生物（低于平均分%.1f分）\n", avgBiology - myScore->biology);
        hasWeak = 1;
    }
    if (!hasWeak) {
        printf("   恭喜！你的所有科目均高于班级平均分！\n");
    }
}

// 从文件加载所有学生数据到链表（核心修复函数）
void loadStudentsToLinkList() {
    // 1. 先清空原有链表，避免重复加载
    Student* temp = stuListHead;
    while (temp != NULL) {
        Student* next = temp->next;
        free(temp);
        temp = next;
    }
    stuListHead = NULL;

    // 2. 打开学生数据文件
    FILE* fp = fopen(STUDENT_FILE, "rb");
    if (fp == NULL) {
        // 文件不存在时创建空文件，避免后续操作报错
        fp = fopen(STUDENT_FILE, "wb");
        if (fp == NULL) {
            printf("学生数据文件创建失败！\n");
            return;
        }
        fclose(fp);
        return;
    }

    // 3. 循环读取文件中的学生数据，构建链表
    Student* tail = NULL; // 链表尾指针（用于尾插法）
    Student stuBuf;       // 临时存储读取的学生数据

    while (fread(&stuBuf, sizeof(Student), 1, fp) == 1) {
        // 为每个学生节点分配内存
        Student* newNode = (Student*)malloc(sizeof(Student));
        if (newNode == NULL) {
            printf("内存分配失败！\n");
            break;
        }

        // 复制文件中的学生数据到新节点
        memcpy(newNode, &stuBuf, sizeof(Student));
        newNode->next = NULL; // 尾节点next置空

        // 尾插法添加到链表
        if (stuListHead == NULL) {
            stuListHead = newNode; // 链表为空时，头节点=新节点
            tail = newNode;
        }
        else {
            tail->next = newNode; // 链表非空时，尾节点指向新节点
            tail = newNode;
        }
    }

    fclose(fp);
    // 加载完成后自动排序，保证排名最新
    sortStudentByTotalScoreByClass();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct Teacher {
    char id[MAX_ID_LEN];           //工号
    char password[PASSWORD_LEN];   //密码
    char name[MAX_NAME_LEN];       //姓名
    char classname[MAX_CLASS_LEN]; //班级
    int age;                       //年龄
    char sex;                   //性别(M/F)
}Teacher;

// 教师登录验证：返回1成功，0失败，-1文件错误
int teacherLogin(char* inputId, char* inputPwd) {
    FILE* fp;
    errno_t err = fopen_s(&fp, TEACHER_FILE, "rb");
    if (err != 0 || fp == NULL) {
        printf("教师账号文件不存在或打开失败！\n");
        return -1;
    }
    Teacher tea;
    // 循环读取每个教师信息
    while (fread(&tea, sizeof(Teacher), 1, fp) == 1) {
        // 比对工号和密码
        if (strcmp(tea.id, inputId) == 0 && strcmp(tea.password, inputPwd) == 0) {
            strcpy(loginTeacherId, tea.id);       // 教师工号
            strcpy(loginTeacherClass, tea.classname);
            // 把教师工号存入全局变量 loginTeacherId
            //strcpy(loginTeacherClass, tea.id);
            fclose(fp);
            printf("登录成功！欢迎你，%s老师\n", tea.name);
            // 额外返回教师管理的班级信息（可选）
            printf("你管理的班级：%s\n", tea.classname);
            loadStudentsToLinkList();
            return 1;
        }
    }
    fclose(fp);
    printf("教师工号或密码错误！\n");
    return 0;
}

// 教师账号注册：返回1成功，0失败（账号已存在），-1文件错误
int teacherRegister() {
    Teacher newTea;
    FILE* fp;
    errno_t err = fopen_s(&fp, TEACHER_FILE, "ab+");  // 追加+二进制模式
    if (err != 0 || fp == NULL) {
        printf("文件打开失败！\n");
        return -1;
    }

    // 1. 输入工号并检查是否重复
    printf("请输入教师工号（账号）：");
    scanf("%s", newTea.id);
    Teacher temp;
    fseek(fp, 0, SEEK_SET);  // 回到文件开头
    while (fread(&temp, sizeof(Teacher), 1, fp) == 1) {
        if (strcmp(temp.id, newTea.id) == 0) {
            fclose(fp);
            printf("该教师工号已注册！\n");
            return 0;
        }
    }

    // 2. 输入教师信息
    printf("请输入教师姓名：");
    scanf("%s", newTea.name);
    printf("请输入你管理的班级ID：");
    scanf("%s", newTea.classname);
    printf("请输入年龄：");
    scanf("%d", &newTea.age);
    printf("请输入性别（M/F）：");
    scanf(" %c", &newTea.sex);

    // 3. 隐藏输入密码（两次确认）
    char pwd1[20], pwd2[20];
    inputPassword(pwd1, 20);
    printf("请再次输入密码：");
    inputPassword(pwd2, 20);
    if (strcmp(pwd1, pwd2) != 0) {
        fclose(fp);
        printf("两次密码输入不一致！\n");
        return 0;
    }
    strcpy(newTea.password, pwd1);

    // 4. 写入文件
    fwrite(&newTea, sizeof(Teacher), 1, fp);
    fclose(fp);
    printf("教师账号注册成功！\n");
    loadStudentsToLinkList(); // 登录后重新加载学生数据
    return 1;
}

// 修改教师密码：输入原工号和原密码，验证通过后修改
int changeTeacherPwd(char* inputId, char* oldPwd) {
    FILE* fp;
    errno_t err = fopen_s(&fp, TEACHER_FILE, "rb+");  // 读写模式
    if (err != 0 || fp == NULL) {
        printf("文件打开失败！\n");
        return -1;
    }
    Teacher tea;
    int found = 0;
    while (fread(&tea, sizeof(Teacher), 1, fp) == 1) {
        if (strcmp(tea.id, inputId) == 0 && strcmp(tea.password, oldPwd) == 0) {
            found = 1;
            // 输入新密码
            char newPwd1[20], newPwd2[20];
            printf("请输入新密码：");
            inputPassword(newPwd1, 20);
            printf("请再次输入新密码：");
            inputPassword(newPwd2, 20);
            if (strcmp(newPwd1, newPwd2) != 0) {
                fclose(fp);
                printf("两次密码不一致！\n");
                return 0;
            }
            // 修复fseek类型警告：显式转换为long
            fseek(fp, -(long)sizeof(Teacher), SEEK_CUR);
            strcpy(tea.password, newPwd1);
            fwrite(&tea, sizeof(Teacher), 1, fp);
            break;
        }
    }
    fclose(fp);
    if (!found) {
        printf("教师工号或原密码错误！\n");
        return 0;
    }
    printf("密码修改成功！\n");
    return 1;
}



// ===================== 教师-新增学生信息 =====================
void teacherAddStudent() {
    Student newStu;
    FILE* fp = fopen(STUDENT_FILE, "ab+");
    if (fp == NULL) {
        printf("文件打开失败！\n");
        return;
    }

    // 1. 输入学号并查重
    printf("请输入新增学生学号：");
    scanf("%s", newStu.id);

    // 查重（全局查重，避免学号重复）
    Student temp;
    fseek(fp, 0, SEEK_SET);
    while (fread(&temp, sizeof(Student), 1, fp) == 1) {
        if (strcmp(temp.id, newStu.id) == 0) {
            fclose(fp);
            printf("该学号已存在！无法新增\n");
            return;
        }
    }

    // 2. 输入学生信息（班级固定为教师管理的班级）
    strcpy(newStu.classname, loginTeacherClass); // 强制绑定教师管理的班级
    printf("请输入学生姓名：");
    scanf("%s", newStu.name);
    printf("请输入学生性别（M/F）：");
    scanf(" %c", &newStu.sex);
    printf("请输入学生年龄：");
    scanf("%d", &newStu.age);

    // 3. 初始化成绩和密码
    newStu.chinese = 0;
    newStu.math = 0;
    newStu.english = 0;
    newStu.physics = 0;
    newStu.chemistry = 0;
    newStu.biology = 0;
    calculateTotalScore(&newStu);
    newStu.rank = 0;
    newStu.next = NULL;

    // 4. 设置初始密码（默认学号后6位，不足补0）
    char initPwd[PASSWORD_LEN] = { 0 };
    int idLen = strlen(newStu.id);
    if (idLen >= 6) {
        strncpy(initPwd, newStu.id + idLen - 6, 6);
    }
    else {
        strcpy(initPwd, newStu.id);
        // 不足6位补0
        for (int i = idLen; i < 6; i++) {
            initPwd[i] = '0';
        }
    }
    strcpy(newStu.password, initPwd);

    // 5. 写入文件
    fwrite(&newStu, sizeof(Student), 1, fp);
    fclose(fp);

    // 6. 更新链表
    loadStudentsToLinkList();
    printf("新增学生成功！\n");
    printf("初始密码：%s（建议提醒学生及时修改）\n", initPwd);
}
// ===================== 教师-删除学生信息 =====================
void teacherDeleteStudent() {
    char delId[MAX_ID_LEN];
    printf("请输入要删除的学生学号：");
    scanf("%s", delId);
    // 1. 先检查是否存在且属于本班
    int found = 0;
    Student* prev = NULL;
    Student* curr = stuListHead;
    while (curr != NULL) {
        if (strcmp(curr->id, delId) == 0 && strcmp(curr->classname, loginTeacherClass) == 0) {
            found = 1;
            break;
        }
        prev = curr;
        curr = curr->next;
    }
    if (!found) {
        printf("未找到该学生，或该学生不属于你管理的班级！\n");
        return;
    }
    // 2. 确认删除
    printf("确认删除 %s（学号：%s）的信息吗？(Y/N)：", curr->name, curr->id);
    char confirm;
    scanf(" %c", &confirm);
    if (confirm != 'Y' && confirm != 'y') {
        printf("取消删除！\n");
        return;
    }
    // 3. 从链表删除节点
    if (prev == NULL) {
        stuListHead = curr->next;
    }
    else {
        prev->next = curr->next;
    }
    free(curr);
    // 4. 从文件删除（重新写入所有非删除的学生）
    FILE* fp = fopen(STUDENT_FILE, "wb");
    if (fp == NULL) {
        printf("文件打开失败！\n");
        return;
    }
    Student* temp = stuListHead;
    while (temp != NULL) {
        Student saveStu = *temp;
        saveStu.next = NULL;
        fwrite(temp, sizeof(Student), 1, fp);
        temp = temp->next;
    }
    fclose(fp);
    sortStudentByTotalScoreByClass();
    printf("删除成功！\n");
}

// ===================== 教师-查询学生信息 =====================
void teacherQueryStudent() {
    printf("请选择查询方式：\n");
    printf("1. 按学号查询\n");
    printf("2. 按姓名查询\n");
    printf("请输入选择：");
    int op;
    scanf("%d", &op);
    clearInputBuffer();
    Student* temp = stuListHead;
    int found = 0;
    if (op == 1) {
        char queryId[MAX_ID_LEN];
        printf("请输入要查询的学生学号：");
        scanf("%s", queryId);
        clearInputBuffer();
        while (temp != NULL) {
            if (strcmp(temp->id, queryId) == 0 && strcmp(temp->classname, loginTeacherClass) == 0) {
                found = 1;
                printf("\n===== 学生信息 =====\n");
                printf("学号：%s\t姓名：%s\t班级：%s\n", temp->id, temp->name, temp->classname);
                printf("性别：%c\t年龄：%d\n", temp->sex, temp->age);
                printf("------------------------\n");
                printf("语文：%d\t数学：%d\t英语：%d\n", temp->chinese, temp->math, temp->english);
                printf("物理：%d\t化学：%d\t生物：%d\n", temp->physics, temp->chemistry, temp->biology);
                printf("------------------------\n");
                printf("总分：%.1f\t排名：第%d名\n", temp->total_score, temp->rank);
                break;
            }
            temp = temp->next;
        }
    }
    else if (op == 2) {
        char queryName[MAX_NAME_LEN];
        printf("请输入要查询的学生姓名：");
        scanf("%[^\n]", queryName); // 支持空格
        clearInputBuffer();
        printf("\n===== 查询结果 =====\n");
        while (temp != NULL) {
            if (strcmp(temp->name, queryName) == 0 && strcmp(temp->classname, loginTeacherClass) == 0) {
                found = 1;
                printf("学号：%s\t姓名：%s\t性别：%c\t年龄：%d\t总分：%.1f\t排名：%d\n",
                    temp->id, temp->name, temp->sex, temp->age, temp->total_score, temp->rank);
            }
            temp = temp->next;
        }
    }
    else {
        printf("输入错误！\n");
        return;
    }
    if (!found) {
        printf("未找到该学生，或该学生不属于你管理的班级！\n");
    }
}

// ===================== 教师-单个录入/修改学生成绩 =====================
void teacherModifyStudentScore() {
    char modId[MAX_ID_LEN];
    printf("请输入要录入/修改成绩的学生学号：");
    scanf("%s", modId);
    clearInputBuffer();
    // 1. 查找本班学生
    Student* temp = stuListHead;
    while (temp != NULL) {
        if (strcmp(temp->id, modId) == 0 && strcmp(temp->classname, loginTeacherClass) == 0) {
            // 2. 显示当前成绩（若为0提示"未录入"）
            printf("\n【%s（学号：%s）】当前成绩：\n", temp->name, temp->id);
            printf("语文：%s\t数学：%s\t英语：%s\n",
                temp->chinese == 0 ? "未录入" : to_string(temp->chinese).c_str(),
                temp->math == 0 ? "未录入" : to_string(temp->math).c_str(),
                temp->english == 0 ? "未录入" : to_string(temp->english).c_str());
            printf("物理：%s\t化学：%s\t生物：%s\n",
                temp->physics == 0 ? "未录入" : to_string(temp->physics).c_str(),
                temp->chemistry == 0 ? "未录入" : to_string(temp->chemistry).c_str(),
                temp->biology == 0 ? "未录入" : to_string(temp->biology).c_str());
            printf("--------------------------------\n");
            // 3. 输入新成绩（按回车保留原成绩，输入-1清空成绩）
            char scoreStr[10];
            int newScore;
            // 语文
            printf("请输入语文成绩（0-100，回车保留，-1清空）：");
            scanf("%[^\n]", scoreStr);
            clearInputBuffer();
            if (strlen(scoreStr) > 0) {
                newScore = atoi(scoreStr);
                if (newScore == -1) temp->chinese = 0;
                else if (newScore >= 0 && newScore <= 100) temp->chinese = newScore;
                else printf("语文成绩输入无效，保留原成绩！\n");
            }
            // 数学
            printf("请输入数学成绩（0-100，回车保留，-1清空）：");
            scanf("%[^\n]", scoreStr);
            clearInputBuffer();
            if (strlen(scoreStr) > 0) {
                newScore = atoi(scoreStr);
                if (newScore == -1) temp->math = 0;
                else if (newScore >= 0 && newScore <= 100) temp->math = newScore;
                else printf("数学成绩输入无效，保留原成绩！\n");
            }
            // 英语
            printf("请输入英语成绩（0-100，回车保留，-1清空）：");
            scanf("%[^\n]", scoreStr);
            clearInputBuffer();
            if (strlen(scoreStr) > 0) {
                newScore = atoi(scoreStr);
                if (newScore == -1) temp->english = 0;
                else if (newScore >= 0 && newScore <= 100) temp->english = newScore;
                else printf("英语成绩输入无效，保留原成绩！\n");
            }
            // 物理
            printf("请输入物理成绩（0-100，回车保留，-1清空）：");
            scanf("%[^\n]", scoreStr);
            clearInputBuffer();
            if (strlen(scoreStr) > 0) {
                newScore = atoi(scoreStr);
                if (newScore == -1) temp->physics = 0;
                else if (newScore >= 0 && newScore <= 100) temp->physics = newScore;
                else printf("物理成绩输入无效，保留原成绩！\n");
            }
            // 化学
            printf("请输入化学成绩（0-100，回车保留，-1清空）：");
            scanf("%[^\n]", scoreStr);
            clearInputBuffer();
            if (strlen(scoreStr) > 0) {
                newScore = atoi(scoreStr);
                if (newScore == -1) temp->chemistry = 0;
                else if (newScore >= 0 && newScore <= 100) temp->chemistry = newScore;
                else printf("化学成绩输入无效，保留原成绩！\n");
            }
            // 生物
            printf("请输入生物成绩（0-100，回车保留，-1清空）：");
            scanf("%[^\n]", scoreStr);
            clearInputBuffer();
            if (strlen(scoreStr) > 0) {
                newScore = atoi(scoreStr);
                if (newScore == -1) temp->biology = 0;
                else if (newScore >= 0 && newScore <= 100) temp->biology = newScore;
                else printf("生物成绩输入无效，保留原成绩！\n");
            }
            // 4. 重新计算总分
            calculateTotalScore(temp);
            // 5. 更新文件中的数据
            FILE* fp;
            fopen_s(&fp, STUDENT_FILE, "rb+");
            if (fp != NULL) {
                Student fileStu;
                while (fread(&fileStu, sizeof(Student), 1, fp) == 1) {
                    if (strcmp(fileStu.id, modId) == 0) {
                        fseek(fp, -(long)sizeof(Student), SEEK_CUR);
                        fwrite(temp, sizeof(Student), 1, fp);
                        break;
                    }
                }
                fclose(fp);
            }
            // 6. 重新排序并更新排名
            sortStudentByTotalScoreByClass(); // 新增按班级排名函数;
            printf("\n成绩录入/修改成功！\n");
            printf("更新后总分：%.1f\t当前排名：第%d名\n", temp->total_score, temp->rank);
            return;
        }
        temp = temp->next;
    }
    printf("未找到该学生，或该学生不属于你管理的班级！\n");
}

// ===================== 教师-本班成绩分析 =====================
void teacherAnalyzeScore() {
    if (stuListHead == NULL) {
        printf("暂无学生成绩数据！\n");
        return;
    }
    // 1. 初始化统计变量
    int classCount = 0; // 本班人数
    // 各科总分、最高分、最低分
    int chineseTotal = 0, chineseMax = 0, chineseMin = 100;
    int mathTotal = 0, mathMax = 0, mathMin = 100;
    int englishTotal = 0, englishMax = 0, englishMin = 100;
    int physicsTotal = 0, physicsMax = 0, physicsMin = 100;
    int chemistryTotal = 0, chemistryMax = 0, chemistryMin = 100;
    int biologyTotal = 0, biologyMax = 0, biologyMin = 100;
    float totalTotal = 0, totalMax = 0, totalMin = 600;
    // 及格人数（60分及以上）
    int chinesePass = 0, mathPass = 0, englishPass = 0;
    int physicsPass = 0, chemistryPass = 0, biologyPass = 0;
    // 2. 遍历本班学生统计数据
    Student* temp = stuListHead;
    while (temp != NULL) {
        if (strcmp(temp->classname, loginTeacherClass) == 0) {
            classCount++;
            // 语文
            chineseTotal += temp->chinese;
            if (temp->chinese > chineseMax) chineseMax = temp->chinese;
            if (temp->chinese < chineseMin) chineseMin = temp->chinese;
            if (temp->chinese >= 60) chinesePass++;
            // 数学
            mathTotal += temp->math;
            if (temp->math > mathMax) mathMax = temp->math;
            if (temp->math < mathMin) mathMin = temp->math;
            if (temp->math >= 60) mathPass++;
            // 英语
            englishTotal += temp->english;
            if (temp->english > englishMax) englishMax = temp->english;
            if (temp->english < englishMin) englishMin = temp->english;
            if (temp->english >= 60) englishPass++;
            // 物理
            physicsTotal += temp->physics;
            if (temp->physics > physicsMax) physicsMax = temp->physics;
            if (temp->physics < physicsMin) physicsMin = temp->physics;
            if (temp->physics >= 60) physicsPass++;
            // 化学
            chemistryTotal += temp->chemistry;
            if (temp->chemistry > chemistryMax) chemistryMax = temp->chemistry;
            if (temp->chemistry < chemistryMin) chemistryMin = temp->chemistry;
            if (temp->chemistry >= 60) chemistryPass++;
            // 生物
            biologyTotal += temp->biology;
            if (temp->biology > biologyMax) biologyMax = temp->biology;
            if (temp->biology < biologyMin) biologyMin = temp->biology;
            if (temp->biology >= 60) biologyPass++;
            // 总分
            totalTotal += temp->total_score;
            if (temp->total_score > totalMax) totalMax = temp->total_score;
            if (temp->total_score < totalMin) totalMin = temp->total_score;
        }
        temp = temp->next;
    }
    if (classCount == 0) {
        printf("本班暂无学生数据！\n");
        return;
    }
    // 3. 计算平均分、及格率
    float chineseAvg = (float)chineseTotal / classCount;
    float mathAvg = (float)mathTotal / classCount;
    float englishAvg = (float)englishTotal / classCount;
    float physicsAvg = (float)physicsTotal / classCount;
    float chemistryAvg = (float)chemistryTotal / classCount;
    float biologyAvg = (float)biologyTotal / classCount;
    float totalAvg = totalTotal / classCount;
    float chinesePassRate = (float)chinesePass / classCount * 100;
    float mathPassRate = (float)mathPass / classCount * 100;
    float englishPassRate = (float)englishPass / classCount * 100;
    float physicsPassRate = (float)physicsPass / classCount * 100;
    float chemistryPassRate = (float)chemistryPass / classCount * 100;
    float biologyPassRate = (float)biologyPass / classCount * 100;
    // 4. 展示分析结果
    printf("\n============ %s 班级成绩分析报告 ==========", loginTeacherClass);
    printf("班级总人数：%d\n", classCount);
    printf("==================================================================\n");
    printf("科目\t平均分\t最高分\t最低分\t及格人数\t及格率\n");
    printf("语文\t%.1f\t%d\t%d\t%d\t\t%.1f%%\n", chineseAvg, chineseMax, chineseMin, chinesePass, chinesePassRate);
    printf("数学\t%.1f\t%d\t%d\t%d\t\t%.1f%%\n", mathAvg, mathMax, mathMin, mathPass, mathPassRate);
    printf("英语\t%.1f\t%d\t%d\t%d\t\t%.1f%%\n", englishAvg, englishMax, englishMin, englishPass, englishPassRate);
    printf("物理\t%.1f\t%d\t%d\t%d\t\t%.1f%%\n", physicsAvg, physicsMax, physicsMin, physicsPass, physicsPassRate);
    printf("化学\t%.1f\t%d\t%d\t%d\t\t%.1f%%\n", chemistryAvg, chemistryMax, chemistryMin, chemistryPass, chemistryPassRate);
    printf("生物\t%.1f\t%d\t%d\t%d\t\t%.1f%%\n", biologyAvg, biologyMax, biologyMin, biologyPass, biologyPassRate);
    printf("==================================================================\n");
    printf("总分平均分：%.1f\t总分最高分：%.1f\t总分最低分：%.1f\n", totalAvg, totalMax, totalMin);
    // 5. 薄弱科目提示
    printf("\n===== 薄弱科目提示（平均分<70）=====\n");
    int hasWeak = 0;
    if (chineseAvg < 70) { printf("语文（平均分%.1f）\n", chineseAvg); hasWeak = 1; }
    if (mathAvg < 70) { printf("数学（平均分%.1f）\n", mathAvg); hasWeak = 1; }
    if (englishAvg < 70) { printf("英语（平均分%.1f）\n", englishAvg); hasWeak = 1; }
    if (physicsAvg < 70) { printf("物理（平均分%.1f）\n", physicsAvg); hasWeak = 1; }
    if (chemistryAvg < 70) { printf("化学（平均分%.1f）\n", chemistryAvg); hasWeak = 1; }
    if (biologyAvg < 70) { printf("生物（平均分%.1f）\n", biologyAvg); hasWeak = 1; }
    if (!hasWeak) { printf("本班各科平均分均≥70，整体成绩优秀！\n"); }
}

// 老师端：下载所管理班级的学生信息到文件
void teacherDownloadStudentInfo() {
    // 1. 校验登录状态（老师是否已登录）
    if (loginTeacherClass[0] == '\0') {
        printf("请先以老师身份登录！\n");
        return;
    }
    // 2. 校验是否有学生数据
    if (stuListHead == NULL) {
        printf("暂无学生数据可下载！\n");
        return;
    }
    // 3. 生成带时间戳的文件名（避免重复）
    char filename[100] = { 0 };
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    // 格式：班级_年-月-日_时-分-秒.txt
    sprintf(filename, "%s_%04d-%02d-%02d_%02d-%02d-%02d.txt",
        loginTeacherClass,
        t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
        t->tm_hour, t->tm_min, t->tm_sec);
    // 4. 打开文件（写入模式，不存在则创建）
    FILE* fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("文件创建失败！无法下载学生信息\n");
        return;
    }
    // 5. 写入文件头部（列名）
    fprintf(fp, "学号,姓名,班级,语文,数学,英语,物理,化学,生物,总分,排名\n");
    fprintf(fp, "========================================================================\n");
    // 6. 遍历链表，筛选当前老师管理的班级数据并写入
    Student* temp = stuListHead;
    int count = 0;  // 统计下载的学生数量
    while (temp != NULL) {
        if (strcmp(temp->classname, loginTeacherClass) == 0) {
            count++;
            // 按CSV格式写入（逗号分隔，方便Excel打开）
            fprintf(fp, "%s,%s,%s,%d,%d,%d,%d,%d,%d,%.1f,%d\n",
                temp->id, temp->name, temp->classname,
                temp->chinese, temp->math, temp->english,
                temp->physics, temp->chemistry, temp->biology,
                temp->total_score, temp->rank);
        }
        temp = temp->next;
    }
    // 7. 关闭文件并提示结果
    fclose(fp);
    if (count == 0) {
        printf("你管理的班级（%s）暂无学生数据，已创建空文件：%s\n", loginTeacherClass, filename);
        // 可选：删除空文件
        // remove(filename);
        // printf("你管理的班级（%s）暂无学生数据，未创建文件\n", loginTeacherClass);
    }
    else {
        printf("下载成功！共导出 %d 名学生信息\n", count);
        printf("文件路径：%s\n", filename);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////

typedef struct Admin {
    char id[MAX_ID_LEN];           //学号
    char password[PASSWORD_LEN];   //密码
    char name[MAX_NAME_LEN];       //姓名
}Admin;
// 管理员登录验证：返回1成功，0失败，-1文件错误
int adminLogin(char* inputId, char* inputPwd) {
    FILE* fp;
    errno_t err = fopen_s(&fp, ADMIN_FILE, "rb");
    if (err != 0 || fp == NULL) {
        printf("管理员账号文件不存在或打开失败！\n");
        return -1;
    }
    Admin admin;
    while (fread(&admin, sizeof(Admin), 1, fp) == 1) {
        // 比对管理员工号和密码
        if (strcmp(admin.id, inputId) == 0 && strcmp(admin.password, inputPwd) == 0) {
            fclose(fp);
            printf("登录成功！欢迎进入管理员后台系统\n");
            printf("【提示】你拥有最高权限，可管理所有账号和成绩数据\n");
            return 1;
        }
    }
    fclose(fp);
    printf("管理员工号或密码错误！\n");
    return 0;
}
// 管理员账号注册：返回1成功，0失败（账号已存在），-1文件错误
// 注：实际项目中管理员账号通常由超级管理员创建，此函数仅用于测试
int adminRegister() {
    Admin newAdmin;
    FILE* fp;
    errno_t err = fopen_s(&fp, ADMIN_FILE, "ab+");
    if (err != 0 || fp == NULL) {
        printf("文件打开失败！\n");
        return -1;
    }
    // 1. 输入管理员工号并检查重复
    printf("请输入管理员工号（账号）：");
    scanf("%s", newAdmin.id);
    Admin temp;
    fseek(fp, 0, SEEK_SET);
    while (fread(&temp, sizeof(Admin), 1, fp) == 1) {
        if (strcmp(temp.id, newAdmin.id) == 0) {
            fclose(fp);
            printf("该管理员工号已注册！\n");
            return 0;
        }
    }

    printf("请输入姓名：");
    scanf("%s", newAdmin.name);

    // 2. 隐藏输入密码（两次确认）
    char pwd1[20], pwd2[20];
    inputPassword(pwd1, 20);
    printf("请再次输入密码：");
    inputPassword(pwd2, 20);
    if (strcmp(pwd1, pwd2) != 0) {
        fclose(fp);
        printf("两次密码输入不一致！\n");
        return 0;
    }
    strcpy(newAdmin.password, pwd1);
    // 3. 写入文件
    fwrite(&newAdmin, sizeof(Admin), 1, fp);
    fclose(fp);
    printf("管理员账号注册成功！\n");
    return 1;
}
// 修改管理员密码：输入原工号和原密码，验证通过后修改
int changeAdminPwd(char* inputId, char* oldPwd) {
    FILE* fp;
    errno_t err = fopen_s(&fp, ADMIN_FILE, "rb+");
    if (err != 0 || fp == NULL) {
        printf("文件打开失败！\n");
        return -1;
    }
    Admin admin;
    int found = 0;
    while (fread(&admin, sizeof(Admin), 1, fp) == 1) {
        if (strcmp(admin.id, inputId) == 0 && strcmp(admin.password, oldPwd) == 0) {
            found = 1;
            // 输入新密码
            char newPwd1[20], newPwd2[20];
            printf("请输入新密码：");
            inputPassword(newPwd1, 20);
            printf("请再次输入新密码：");
            inputPassword(newPwd2, 20);
            if (strcmp(newPwd1, newPwd2) != 0) {
                fclose(fp);
                printf("两次密码不一致！\n");
                return 0;
            }
            // 修复fseek类型警告：显式转换为long
            fseek(fp, -(long)sizeof(Admin), SEEK_CUR);
            strcpy(admin.password, newPwd1);
            fwrite(&admin, sizeof(Admin), 1, fp);
            break;
        }
    }
    fclose(fp);
    if (!found) {
        printf("管理员工号或原密码错误！\n");
        return 0;
    }
    printf("密码修改成功！\n");
    return 1;
}

// ===================== 密码找回申请相关函数 =====================

// 检查是否已有待审批的申请
int hasPendingRequest(const char* id, int role) {
    FILE* fp = fopen(PWD_REQUEST_FILE, "rb");
    if (!fp) return 0;

    PwdRequest req;
    while (fread(&req, sizeof(PwdRequest), 1, fp) == 1) {
        if (strcmp(req.id, id) == 0 && req.role == role && req.status == 0) {
            fclose(fp);
            return 1;  // 有未处理的申请
        }
    }
    fclose(fp);
    return 0;
}

// 提交密码找回申请（学生）
void submitStudentPwdRequest() {
    char inputId[MAX_ID_LEN];
    char inputName[MAX_NAME_LEN];
    char inputClass[MAX_CLASS_LEN];

    printf("\n========== 密码找回申请（学生） ==========\n");
    printf("请输入学号：");
    scanf("%s", inputId);
    clearInputBuffer();

    // 验证学生是否存在
    FILE* fp = fopen(STUDENT_FILE, "rb");
    if (!fp) {
        printf("系统错误！\n");
        return;
    }

    Student stu;
    int found = 0;
    while (fread(&stu, sizeof(Student), 1, fp) == 1) {
        if (strcmp(stu.id, inputId) == 0) {
            found = 1;
            strcpy(inputName, stu.name);
            strcpy(inputClass, stu.classname);
            break;
        }
    }
    fclose(fp);

    if (!found) {
        printf("该学号不存在！\n");
        return;
    }

    // 检查是否已有待审批申请
    if (hasPendingRequest(inputId, 1)) {
        printf("您已提交过密码找回申请，请耐心等待管理员审批。\n");
        printf("请勿重复提交！\n");
        return;
    }

    // 验证身份信息
    printf("请输入姓名进行验证：");
    char verifyName[MAX_NAME_LEN];
    scanf("%s", verifyName);
    clearInputBuffer();

    printf("请输入班级进行验证：");
    char verifyClass[MAX_CLASS_LEN];
    scanf("%s", verifyClass);
    clearInputBuffer();

    if (strcmp(verifyName, inputName) != 0 || strcmp(verifyClass, inputClass) != 0) {
        printf("身份验证失败！信息不匹配。\n");
        return;
    }

    // 创建申请
    PwdRequest req;
    strcpy(req.id, inputId);
    strcpy(req.name, inputName);
    strcpy(req.classname, inputClass);
    req.role = 1;  // 学生
    req.status = 0;  // 待审批
    req.requestTime = time(NULL);

    // 保存申请
    FILE* reqFp = fopen(PWD_REQUEST_FILE, "ab");
    if (!reqFp) {
        printf("系统错误，无法提交申请！\n");
        return;
    }
    fwrite(&req, sizeof(PwdRequest), 1, reqFp);
    fclose(reqFp);

    printf("\n申请提交成功！\n");
    printf("您的密码找回申请已发送至管理员，审批通过后可使用初始密码登录。\n");
    printf("初始密码规则：学号后6位（不足补0）\n");
    printf("请留意审批结果，登录后请及时修改密码。\n");
}

// 提交密码找回申请（教师）
void submitTeacherPwdRequest() {
    char inputId[MAX_ID_LEN];

    printf("\n========== 密码找回申请（教师） ==========\n");
    printf("请输入工号：");
    scanf("%s", inputId);
    clearInputBuffer();

    // 验证教师是否存在
    FILE* fp = fopen(TEACHER_FILE, "rb");
    if (!fp) {
        printf("系统错误！\n");
        return;
    }

    Teacher tea;
    int found = 0;
    char inputName[MAX_NAME_LEN];
    char inputClass[MAX_CLASS_LEN];

    while (fread(&tea, sizeof(Teacher), 1, fp) == 1) {
        if (strcmp(tea.id, inputId) == 0) {
            found = 1;
            strcpy(inputName, tea.name);
            strcpy(inputClass, tea.classname);
            break;
        }
    }
    fclose(fp);

    if (!found) {
        printf("该工号不存在！\n");
        return;
    }

    // 检查是否已有待审批申请
    if (hasPendingRequest(inputId, 2)) {
        printf("您已提交过密码找回申请，请耐心等待管理员审批。\n");
        printf("请勿重复提交！\n");
        return;
    }

    // 验证身份信息
    printf("请输入姓名进行验证：");
    char verifyName[MAX_NAME_LEN];
    scanf("%s", verifyName);
    clearInputBuffer();

    if (strcmp(verifyName, inputName) != 0) {
        printf("身份验证失败！姓名不匹配。\n");
        return;
    }

    // 创建申请
    PwdRequest req;
    strcpy(req.id, inputId);
    strcpy(req.name, inputName);
    strcpy(req.classname, inputClass);
    req.role = 2;  // 教师
    req.status = 0;  // 待审批
    req.requestTime = time(NULL);

    // 保存申请
    FILE* reqFp = fopen(PWD_REQUEST_FILE, "ab");
    if (!reqFp) {
        printf("系统错误，无法提交申请！\n");
        return;
    }
    fwrite(&req, sizeof(PwdRequest), 1, reqFp);
    fclose(reqFp);

    printf("\n申请提交成功！\n");
    printf("您的密码找回申请已发送至管理员，审批通过后可使用初始密码登录。\n");
    printf("初始密码规则：工号后6位（不足补0）\n");
    printf("请留意审批结果，登录后请及时修改密码。\n");
}

// 生成默认密码（学号/工号后6位，不足补0）
void generateDefaultPwd(const char* id, char* pwd) {
    int len = strlen(id);
    if (len >= 6) {
        strncpy(pwd, id + len - 6, 6);
        pwd[6] = '\0';
    }
    else {
        strcpy(pwd, id);
        for (int i = len; i < 6; i++) {
            pwd[i] = '0';
        }
        pwd[6] = '\0';
    }
}

// 重置学生密码为默认密码
int resetStudentPassword(const char* id) {
    FILE* fp;
    errno_t err = fopen_s(&fp, STUDENT_FILE, "rb+");
    if (err != 0 || fp == NULL) return 0;

    Student stu;
    while (fread(&stu, sizeof(Student), 1, fp) == 1) {
        if (strcmp(stu.id, id) == 0) {
            char defaultPwd[PASSWORD_LEN];
            generateDefaultPwd(id, defaultPwd);

            fseek(fp, -(long)sizeof(Student), SEEK_CUR);
            strcpy(stu.password, defaultPwd);
            fwrite(&stu, sizeof(Student), 1, fp);
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

// 重置教师密码为默认密码
int resetTeacherPassword(const char* id) {
    FILE* fp;
    errno_t err = fopen_s(&fp, TEACHER_FILE, "rb+");
    if (err != 0 || fp == NULL) return 0;

    Teacher tea;
    while (fread(&tea, sizeof(Teacher), 1, fp) == 1) {
        if (strcmp(tea.id, id) == 0) {
            char defaultPwd[PASSWORD_LEN];
            generateDefaultPwd(id, defaultPwd);

            fseek(fp, -(long)sizeof(Teacher), SEEK_CUR);
            strcpy(tea.password, defaultPwd);
            fwrite(&tea, sizeof(Teacher), 1, fp);
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

// 更新申请状态
void updateRequestStatus(const char* id, int role, int newStatus) {
    FILE* fp = fopen(PWD_REQUEST_FILE, "rb+");
    if (!fp) return;

    PwdRequest req;
    while (fread(&req, sizeof(PwdRequest), 1, fp) == 1) {
        if (strcmp(req.id, id) == 0 && req.role == role && req.status == 0) {
            fseek(fp, -(long)sizeof(PwdRequest), SEEK_CUR);
            req.status = newStatus;
            fwrite(&req, sizeof(PwdRequest), 1, fp);
            break;
        }
    }
    fclose(fp);
}

// 管理员查看并处理密码找回申请
void adminProcessPwdRequests() {
    FILE* fp = fopen(PWD_REQUEST_FILE, "rb");
    if (!fp) {
        printf("暂无密码找回申请。\n");
        return;
    }

    // 先统计待审批数量
    int pendingCount = 0;
    PwdRequest req;
    while (fread(&req, sizeof(PwdRequest), 1, fp) == 1) {
        if (req.status == 0) pendingCount++;
    }

    if (pendingCount == 0) {
        printf("暂无待审批的密码找回申请。\n");
        fclose(fp);
        return;
    }

    printf("\n========== 密码找回申请审批（共%d条待处理） ==========\n", pendingCount);

    // 回到文件开头重新读取
    fseek(fp, 0, SEEK_SET);

    int index = 1;
    while (fread(&req, sizeof(PwdRequest), 1, fp) == 1) {
        if (req.status != 0) continue;  // 只显示待审批的

        printf("\n【申请 %d】\n", index++);
        printf("类型：%s\n", req.role == 1 ? "学生" : "教师");
        printf("账号：%s\n", req.id);
        printf("姓名：%s\n", req.name);
        if (req.role == 1) {
            printf("班级：%s\n", req.classname);
        }

        // 转换时间
        char timeStr[26];
        ctime_s(timeStr, sizeof(timeStr), &req.requestTime);
        printf("申请时间：%s", timeStr);

        printf("\n请选择操作：\n");
        printf("1. 批准（重置为默认密码）\n");
        printf("2. 拒绝\n");
        printf("3. 跳过，处理下一条\n");
        printf("4. 退出审批\n");
        printf("请选择：");

        int choice;
        scanf("%d", &choice);
        clearInputBuffer();

        if (choice == 1) {
            // 批准
            int success = 0;
            char defaultPwd[PASSWORD_LEN];
            generateDefaultPwd(req.id, defaultPwd);

            if (req.role == 1) {
                success = resetStudentPassword(req.id);
            }
            else {
                success = resetTeacherPassword(req.id);
            }

            if (success) {
                updateRequestStatus(req.id, req.role, 1);  // 标记为已批准
                printf("✓ 已批准！密码已重置为：%s\n", defaultPwd);
            }
            else {
                printf("× 重置密码失败，请检查数据文件。\n");
            }
        }
        else if (choice == 2) {
            // 拒绝
            updateRequestStatus(req.id, req.role, 2);  // 标记为已拒绝
            printf("✓ 已拒绝该申请。\n");
        }
        else if (choice == 3) {
            // 跳过
            printf("已跳过。\n");
            continue;
        }
        else if (choice == 4) {
            // 退出
            printf("退出审批模式。\n");
            break;
        }
        else {
            printf("无效选择，自动跳过。\n");
        }
    }

    fclose(fp);

    // 清理已处理的申请（可选：将已审批的移出或标记）
    printf("\n审批处理完成！\n");
}

// ===================== 修改后的统计信息显示 =====================
void adminViewDashboard() {
    // 统计学生
    int stuCount = 0, noScoreCount = 0;
    Student* temp = stuListHead;
    while (temp) {
        stuCount++;
        if (temp->chinese == 0 && temp->math == 0 && temp->english == 0 &&
            temp->physics == 0 && temp->chemistry == 0 && temp->biology == 0)
            noScoreCount++;
        temp = temp->next;
    }

    // 统计教师
    FILE* fp = fopen(TEACHER_FILE, "rb");
    int teaCount = 0;
    Teacher tea;
    if (fp) {
        while (fread(&tea, sizeof(Teacher), 1, fp) == 1) teaCount++;
        fclose(fp);
    }

    // 统计管理员
    fp = fopen(ADMIN_FILE, "rb");
    int adminCount = 0;
    Admin adm;
    if (fp) {
        while (fread(&adm, sizeof(Admin), 1, fp) == 1) adminCount++;
        fclose(fp);
    }

    // 统计待审批的密码找回申请
    fp = fopen(PWD_REQUEST_FILE, "rb");
    int pendingRequests = 0;
    PwdRequest req;
    if (fp) {
        while (fread(&req, sizeof(PwdRequest), 1, fp) == 1) {
            if (req.status == 0) pendingRequests++;
        }
        fclose(fp);
    }

    printf("\n========== 系统待办与统计 ==========\n");
    printf("学生总数：%d\n", stuCount);
    printf("教师总数：%d\n", teaCount);
    printf("管理员总数：%d\n", adminCount);
    printf("未录入成绩的学生数：%d\n", noScoreCount);
    printf("\n>>> 待审批密码找回申请：%d <<<\n", pendingRequests);
    if (pendingRequests > 0) {
        printf("【提示】有用户等待密码重置审批，请及时处理！\n");
    }
    printf("====================================\n");
}

// 通用函数：检查账号是否存在（用于增删改查时的重复性校验）
int accountExists(const char* id, const char* role) {
    FILE* fp = NULL;
    if (strcmp(role, "student") == 0) fp = fopen(STUDENT_FILE, "rb");
    else if (strcmp(role, "teacher") == 0) fp = fopen(TEACHER_FILE, "rb");
    else if (strcmp(role, "admin") == 0) fp = fopen(ADMIN_FILE, "rb");
    else return 0;

    if (!fp) return 0;
    if (strcmp(role, "student") == 0) {
        Student s;
        while (fread(&s, sizeof(Student), 1, fp) == 1)
            if (strcmp(s.id, id) == 0) { fclose(fp); return 1; }
    }
    else if (strcmp(role, "teacher") == 0) {
        Teacher t;
        while (fread(&t, sizeof(Teacher), 1, fp) == 1)
            if (strcmp(t.id, id) == 0) { fclose(fp); return 1; }
    }
    else if (strcmp(role, "admin") == 0) {
        Admin a;
        while (fread(&a, sizeof(Admin), 1, fp) == 1)
            if (strcmp(a.id, id) == 0) { fclose(fp); return 1; }
    }
    fclose(fp);
    return 0;
}

// 管理员增加账号
void adminAddAccount() {
    printf("\n请选择要添加的账号类型：\n");
    printf("1. 学生\n2. 教师\n3. 管理员\n请选择：");
    int type; scanf("%d", &type); clearInputBuffer();

    char id[20];
    printf("请输入账号（学号/工号）：");
    scanf("%s", id); clearInputBuffer();

    if (accountExists(id, type == 1 ? "student" : (type == 2 ? "teacher" : "admin"))) {
        printf("该账号已存在！\n");
        return;
    }

    if (type == 1) { // 学生
        Student newStu;
        strcpy(newStu.id, id);
        printf("请输入姓名："); scanf("%s", newStu.name);
        printf("请输入班级："); scanf("%s", newStu.classname);
        printf("请输入年龄："); scanf("%d", &newStu.age);
        printf("请输入性别(M/F)："); scanf(" %c", &newStu.sex);
        newStu.chinese = newStu.math = newStu.english = newStu.physics = newStu.chemistry = newStu.biology = 0;
        calculateTotalScore(&newStu);
        newStu.rank = 0;
        // 默认密码：学号后6位
        char pwd[PASSWORD_LEN] = { 0 };
        int len = strlen(newStu.id);
        if (len >= 6) strncpy(pwd, newStu.id + len - 6, 6);
        else { strcpy(pwd, newStu.id); for (int i = len; i < 6; i++) pwd[i] = '0'; }
        strcpy(newStu.password, pwd);

        FILE* fp = fopen(STUDENT_FILE, "ab");
        if (fp) { fwrite(&newStu, sizeof(Student), 1, fp); fclose(fp); }
        printf("学生添加成功！初始密码：%s\n", pwd);
        loadStudentsToLinkList();
    }
    else if (type == 2) { // 教师
        Teacher newTea;
        strcpy(newTea.id, id);
        printf("请输入姓名："); scanf("%s", newTea.name);
        printf("请输入管理的班级："); scanf("%s", newTea.classname);
        printf("请输入年龄："); scanf("%d", &newTea.age);
        printf("请输入性别(M/F)："); scanf(" %c", &newTea.sex);
        // 默认密码：工号后6位
        char pwd[PASSWORD_LEN] = { 0 };
        int len = strlen(newTea.id);
        if (len >= 6) strncpy(pwd, newTea.id + len - 6, 6);
        else { strcpy(pwd, newTea.id); for (int i = len; i < 6; i++) pwd[i] = '0'; }
        strcpy(newTea.password, pwd);

        FILE* fp = fopen(TEACHER_FILE, "ab");
        if (fp) { fwrite(&newTea, sizeof(Teacher), 1, fp); fclose(fp); }
        printf("教师添加成功！初始密码：%s\n", pwd);
    }
    else if (type == 3) { // 管理员
        Admin newAd;
        strcpy(newAd.id, id);
        printf("请输入姓名："); scanf("%s", newAd.name);
        // 默认密码：工号后6位
        char pwd[PASSWORD_LEN] = { 0 };
        int len = strlen(newAd.id);
        if (len >= 6) strncpy(pwd, newAd.id + len - 6, 6);
        else { strcpy(pwd, newAd.id); for (int i = len; i < 6; i++) pwd[i] = '0'; }
        strcpy(newAd.password, pwd);

        FILE* fp = fopen(ADMIN_FILE, "ab");
        if (fp) { fwrite(&newAd, sizeof(Admin), 1, fp); fclose(fp); }
        printf("管理员添加成功！初始密码：%s\n", pwd);
    }
}

// 管理员删除账号
void adminDeleteAccount() {
    printf("\n请选择要删除的账号类型：\n");
    printf("1. 学生\n2. 教师\n3. 管理员\n请选择：");
    int type; scanf("%d", &type); clearInputBuffer();

    char id[20];
    printf("请输入要删除的账号：");
    scanf("%s", id); clearInputBuffer();

    const char* role = type == 1 ? "student" : (type == 2 ? "teacher" : "admin");
    if (!accountExists(id, role)) {
        printf("该账号不存在！\n");
        return;
    }

    // 确认删除
    printf("确认删除该账号？(y/n)：");
    char confirm; scanf(" %c", &confirm);
    if (confirm != 'y' && confirm != 'Y') { printf("取消删除\n"); return; }

    FILE* fp = NULL;
    FILE* tempFp = tmpfile(); // 临时文件，或使用实际文件重命名
    if (type == 1) {
        fp = fopen(STUDENT_FILE, "rb");
        if (!fp) return;
        Student s;
        FILE* newFp = fopen("temp_stu.bin", "wb");
        while (fread(&s, sizeof(Student), 1, fp) == 1) {
            if (strcmp(s.id, id) != 0)
                fwrite(&s, sizeof(Student), 1, newFp);
        }
        fclose(fp); fclose(newFp);
        remove(STUDENT_FILE);
        rename("temp_stu.bin", STUDENT_FILE);
        loadStudentsToLinkList();
    }
    else if (type == 2) {
        fp = fopen(TEACHER_FILE, "rb");
        if (!fp) return;
        Teacher t;
        FILE* newFp = fopen("temp_tea.bin", "wb");
        while (fread(&t, sizeof(Teacher), 1, fp) == 1) {
            if (strcmp(t.id, id) != 0)
                fwrite(&t, sizeof(Teacher), 1, newFp);
        }
        fclose(fp); fclose(newFp);
        remove(TEACHER_FILE);
        rename("temp_tea.bin", TEACHER_FILE);
    }
    else if (type == 3) {
        fp = fopen(ADMIN_FILE, "rb");
        if (!fp) return;
        Admin a;
        FILE* newFp = fopen("temp_adm.bin", "wb");
        while (fread(&a, sizeof(Admin), 1, fp) == 1) {
            if (strcmp(a.id, id) != 0)
                fwrite(&a, sizeof(Admin), 1, newFp);
        }
        fclose(fp); fclose(newFp);
        remove(ADMIN_FILE);
        rename("temp_adm.bin", ADMIN_FILE);
    }
    printf("删除成功！\n");
}

// 管理员修改账号信息（不包括密码，如需重置密码可单独处理）
void adminModifyAccount() {
    printf("\n请选择要修改的账号类型：\n");
    printf("1. 学生\n2. 教师\n3. 管理员\n请选择：");
    int type; scanf("%d", &type); clearInputBuffer();
    char id[20];
    printf("请输入要修改的账号：");
    scanf("%s", id); clearInputBuffer();
    const char* role = type == 1 ? "student" : (type == 2 ? "teacher" : "admin");
    if (!accountExists(id, role)) {
        printf("该账号不存在！\n");
        return;
    }

    // 读取原数据并修改
    FILE* fp = NULL;
    long pos;
    if (type == 1) {
        fp = fopen(STUDENT_FILE, "rb+");
        if (!fp) return;
        Student s;
        while (fread(&s, sizeof(Student), 1, fp) == 1) {
            if (strcmp(s.id, id) == 0) {
                pos = ftell(fp) - (long)sizeof(Student);
                printf("当前信息：姓名=%s 班级=%s 年龄=%d 性别=%c\n", s.name, s.classname, s.age, s.sex);
                printf("请输入新姓名（回车保留）：");
                char buf[50]; scanf("%[^\n]", buf); clearInputBuffer();
                if (strlen(buf) > 0) strcpy(s.name, buf);
                printf("请输入新班级（回车保留）：");
                scanf("%[^\n]", buf); clearInputBuffer();
                if (strlen(buf) > 0) strcpy(s.classname, buf);
                printf("请输入新年龄（回车保留）：");
                scanf("%[^\n]", buf); clearInputBuffer();
                if (strlen(buf) > 0) s.age = atoi(buf);
                printf("请输入新性别(M/F)（回车保留）：");
                scanf(" %c", &buf[0]); clearInputBuffer();
                if (buf[0] != '\n') s.sex = buf[0];

                fseek(fp, pos, SEEK_SET);
                fwrite(&s, sizeof(Student), 1, fp);
                break;
            }
        }
        fclose(fp);
        loadStudentsToLinkList();
    }
    else if (type == 2) {
        fp = fopen(TEACHER_FILE, "rb+");
        if (!fp) return;
        Teacher t;
        while (fread(&t, sizeof(Teacher), 1, fp) == 1) {
            if (strcmp(t.id, id) == 0) {
                pos = ftell(fp) - (long)sizeof(Teacher);
                printf("当前信息：姓名=%s 班级=%s 年龄=%d 性别=%c\n", t.name, t.classname, t.age, t.sex);
                char buf[50];
                printf("请输入新姓名（回车保留）：");
                scanf("%[^\n]", buf); clearInputBuffer();
                if (strlen(buf) > 0) strcpy(t.name, buf);
                printf("请输入新班级（回车保留）：");
                scanf("%[^\n]", buf); clearInputBuffer();
                if (strlen(buf) > 0) strcpy(t.classname, buf);
                printf("请输入新年龄（回车保留）：");
                scanf("%[^\n]", buf); clearInputBuffer();
                if (strlen(buf) > 0) t.age = atoi(buf);
                printf("请输入新性别(M/F)（回车保留）：");
                scanf(" %c", &buf[0]); clearInputBuffer();
                if (buf[0] != '\n') t.sex = buf[0];

                fseek(fp, pos, SEEK_SET);
                fwrite(&t, sizeof(Teacher), 1, fp);
                break;
            }
        }
        fclose(fp);
    }
    else if (type == 3) {
        fp = fopen(ADMIN_FILE, "rb+");
        if (!fp) return;
        Admin a;
        while (fread(&a, sizeof(Admin), 1, fp) == 1) {
            if (strcmp(a.id, id) == 0) {
                pos = ftell(fp) - (long)sizeof(Admin);
                printf("当前姓名：%s\n", a.name);
                printf("请输入新姓名（回车保留）：");
                char buf[50]; scanf("%[^\n]", buf); clearInputBuffer();
                if (strlen(buf) > 0) strcpy(a.name, buf);
                fseek(fp, pos, SEEK_SET);
                fwrite(&a, sizeof(Admin), 1, fp);
                break;
            }
        }
        fclose(fp);
    }
    printf("修改成功！\n");
}

// 管理员查询账号
void adminQueryAccount() {
    printf("\n请选择要查询的账号类型：\n");
    printf("1. 学生\n2. 教师\n3. 管理员\n请选择：");
    int type; scanf("%d", &type); clearInputBuffer();

    printf("查询方式：1.按账号  2.按姓名\n请选择：");
    int method; scanf("%d", &method); clearInputBuffer();

    char keyword[50];
    if (method == 1) printf("请输入账号：");
    else printf("请输入姓名：");
    scanf("%[^\n]", keyword); clearInputBuffer();

    int found = 0;
    if (type == 1) {
        Student* temp = stuListHead;
        while (temp) {
            if ((method == 1 && strcmp(temp->id, keyword) == 0) ||
                (method == 2 && strcmp(temp->name, keyword) == 0)) {
                printf("学号：%s 姓名：%s 班级：%s 年龄：%d 性别：%c 总分：%.1f 排名：%d\n",
                    temp->id, temp->name, temp->classname, temp->age, temp->sex, temp->total_score, temp->rank);
                found = 1;
            }
            temp = temp->next;
        }
    }
    else if (type == 2) {
        FILE* fp = fopen(TEACHER_FILE, "rb");
        if (fp) {
            Teacher t;
            while (fread(&t, sizeof(Teacher), 1, fp) == 1) {
                if ((method == 1 && strcmp(t.id, keyword) == 0) ||
                    (method == 2 && strcmp(t.name, keyword) == 0)) {
                    printf("工号：%s 姓名：%s 班级：%s 年龄：%d 性别：%c\n",
                        t.id, t.name, t.classname, t.age, t.sex);
                    found = 1;
                }
            }
            fclose(fp);
        }
    }
    else if (type == 3) {
        FILE* fp = fopen(ADMIN_FILE, "rb");
        if (fp) {
            Admin a;
            while (fread(&a, sizeof(Admin), 1, fp) == 1) {
                if ((method == 1 && strcmp(a.id, keyword) == 0) ||
                    (method == 2 && strcmp(a.name, keyword) == 0)) {
                    printf("工号：%s 姓名：%s\n", a.id, a.name);
                    found = 1;
                }
            }
            fclose(fp);
        }
    }
    if (!found) printf("未找到匹配的账号！\n");
}

// 管理员增删改查菜单
void adminManageAccounts() {
    while (1) {
        system("cls");
        printf("*************************************************\n");
        printf("**           账号增删改查管理                  **\n");
        printf("**                                             **\n");
        printf("**         1. 增加账号                         **\n");
        printf("**         2. 删除账号                         **\n");
        printf("**         3. 修改账号信息                     **\n");
        printf("**         4. 查询账号                         **\n");
        printf("**         5. 返回上一级                       **\n");
        printf("**                                             **\n");
        printf("*************************************************\n");
        printf("请选择操作：");
        int op; scanf("%d", &op); clearInputBuffer();

        switch (op) {
        case 1: adminAddAccount(); break;
        case 2: adminDeleteAccount(); break;
        case 3: adminModifyAccount(); break;
        case 4: adminQueryAccount(); break;
        case 5: return;
        default: printf("无效选择！\n"); break;
        }
        printf("\n按任意键继续...");
        _getch();
    }
}

// 从文件导入账号（CSV格式）
void adminImportFromFile() {
    printf("\n请选择导入的账号类型：\n");
    printf("1. 学生\n2. 教师\n3. 管理员\n请选择：");
    int type; scanf("%d", &type); clearInputBuffer();

    char filename[100];
    printf("请输入CSV文件路径：");
    scanf("%s", filename); clearInputBuffer();

    FILE* csv = fopen(filename, "r");
    if (!csv) { printf("文件打开失败！\n"); return; }

    char line[256];
    int success = 0, fail = 0;
    while (fgets(line, sizeof(line), csv)) {
        // 跳过空行和标题行（可选，这里简单处理，假设每行都是有效数据）
        if (line[0] == '\n' || line[0] == '#') continue;
        char* token;
        char* next_token = NULL;
        if (type == 1) { // 学生格式：学号,姓名,班级,年龄,性别
            Student s;
            token = strtok_s(line, ",", &next_token);
            if (!token) continue;
            strcpy(s.id, token);
            token = strtok_s(NULL, ",", &next_token); strcpy(s.name, token);
            token = strtok_s(NULL, ",", &next_token); strcpy(s.classname, token);
            token = strtok_s(NULL, ",", &next_token); s.age = atoi(token);
            token = strtok_s(NULL, ",", &next_token); s.sex = token[0];
            s.chinese = s.math = s.english = s.physics = s.chemistry = s.biology = 0;
            calculateTotalScore(&s);
            s.rank = 0;
            // 默认密码
            char pwd[PASSWORD_LEN] = { 0 };
            int len = strlen(s.id);
            if (len >= 6) strncpy(pwd, s.id + len - 6, 6);
            else { strcpy(pwd, s.id); for (int i = len; i < 6; i++) pwd[i] = '0'; }
            strcpy(s.password, pwd);

            if (!accountExists(s.id, "student")) {
                FILE* fp = fopen(STUDENT_FILE, "ab");
                if (fp) { fwrite(&s, sizeof(Student), 1, fp); fclose(fp); success++; }
                else fail++;
            }
            else fail++;
        }
        else if (type == 2) { // 教师格式：工号,姓名,班级,年龄,性别
            Teacher t;
            token = strtok_s(line, ",", &next_token);
            if (!token) continue;
            strcpy(t.id, token);
            token = strtok_s(NULL, ",", &next_token); strcpy(t.name, token);
            token = strtok_s(NULL, ",", &next_token); strcpy(t.classname, token);
            token = strtok_s(NULL, ",", &next_token); t.age = atoi(token);
            token = strtok_s(NULL, ",", &next_token); t.sex = token[0];
            char pwd[PASSWORD_LEN] = { 0 };
            int len = strlen(t.id);
            if (len >= 6) strncpy(pwd, t.id + len - 6, 6);
            else { strcpy(pwd, t.id); for (int i = len; i < 6; i++) pwd[i] = '0'; }
            strcpy(t.password, pwd);

            if (!accountExists(t.id, "teacher")) {
                FILE* fp = fopen(TEACHER_FILE, "ab");
                if (fp) { fwrite(&t, sizeof(Teacher), 1, fp); fclose(fp); success++; }
                else fail++;
            }
            else fail++;
        }
        else if (type == 3) { // 管理员格式：工号,姓名
            Admin a;
            token = strtok_s(line, ",", &next_token);
            if (!token) continue;
            strcpy(a.id, token);
            token = strtok_s(NULL, ",", &next_token); strcpy(a.name, token);
            char pwd[PASSWORD_LEN] = { 0 };
            int len = strlen(a.id);
            if (len >= 6) strncpy(pwd, a.id + len - 6, 6);
            else { strcpy(pwd, a.id); for (int i = len; i < 6; i++) pwd[i] = '0'; }
            strcpy(a.password, pwd);

            if (!accountExists(a.id, "admin")) {
                FILE* fp = fopen(ADMIN_FILE, "ab");
                if (fp) { fwrite(&a, sizeof(Admin), 1, fp); fclose(fp); success++; }
                else fail++;
            }
            else fail++;
        }
    }
    fclose(csv);
    if (type == 1) loadStudentsToLinkList();
    printf("导入完成：成功 %d 条，失败 %d 条（账号重复或文件错误）\n", success, fail);
}

// 导出账号到CSV文件
void adminExportToFile() {
    printf("\n请选择导出的账号类型：\n");
    printf("1. 学生\n2. 教师\n3. 管理员\n请选择：");
    int type; scanf("%d", &type); clearInputBuffer();

    char filename[100];
    printf("请输入导出CSV文件路径：");
    scanf("%s", filename); clearInputBuffer();

    FILE* csv = fopen(filename, "w");
    if (!csv) { printf("文件创建失败！\n"); return; }

    int count = 0;
    if (type == 1) {
        fprintf(csv, "学号,姓名,班级,年龄,性别,语文,数学,英语,物理,化学,生物,总分,排名\n");
        Student* temp = stuListHead;
        while (temp) {
            fprintf(csv, "%s,%s,%s,%d,%c,%d,%d,%d,%d,%d,%d,%.1f,%d\n",
                temp->id, temp->name, temp->classname, temp->age, temp->sex,
                temp->chinese, temp->math, temp->english, temp->physics, temp->chemistry, temp->biology,
                temp->total_score, temp->rank);
            count++;
            temp = temp->next;
        }
    }
    else if (type == 2) {
        fprintf(csv, "工号,姓名,班级,年龄,性别\n");
        FILE* fp = fopen(TEACHER_FILE, "rb");
        if (fp) {
            Teacher t;
            while (fread(&t, sizeof(Teacher), 1, fp) == 1) {
                fprintf(csv, "%s,%s,%s,%d,%c\n", t.id, t.name, t.classname, t.age, t.sex);
                count++;
            }
            fclose(fp);
        }
    }
    else if (type == 3) {
        fprintf(csv, "工号,姓名\n");
        FILE* fp = fopen(ADMIN_FILE, "rb");
        if (fp) {
            Admin a;
            while (fread(&a, sizeof(Admin), 1, fp) == 1) {
                fprintf(csv, "%s,%s\n", a.id, a.name);
                count++;
            }
            fclose(fp);
        }
    }
    fclose(csv);
    printf("导出成功！共导出 %d 条记录到 %s\n", count, filename);
}

/////////////////////////////////////////////////////////////////////////////////////////
void welcomeScreen() {
    cout << "*************************************************" << endl;
    cout << "*************************************************" << endl;
    cout << "**                                             **" << endl;
    cout << "**                  欢迎使用！                 **" << endl;
    cout << "**               学生管理系统V1.0              **" << endl;
    cout << "**                                             **" << endl;
    cout << "**   1.学生登录   2.教师登录   3.管理员登录    **" << endl;
    cout << "**                                             **" << endl;
    cout << "**                                             **" << endl;
    cout << "**                  Welcome！                  **" << endl;
    cout << "**                                             **" << endl;
    cout << "*************************************************" << endl;
    cout << "*************************************************" << endl;
}

///////////////////////////////////////////////////////////////////////////////////////
void loginscreen() {
    cout << "*************************************************" << endl;
    cout << "*************************************************" << endl;
    cout << "**                     登录                    **" << endl;
    cout << "**                                             **" << endl;
    cout << "**              1.输入账号与密码               **" << endl;
    cout << "**                2. 密码找回                  **" << endl;
    cout << "**                3. 密码修改                  **" << endl;
    cout << "**                4. 密码注册                  **" << endl;
    cout << "**                5.返回上一级                 **" << endl;
    cout << "**                                             **" << endl;
    cout << "*************************************************" << endl;
    cout << "*************************************************" << endl;
}
////////////////////////////////////////////////////////////////////////////////////
void studentMainscreen() {
    cout << "*************************************************" << endl;
    cout << "*************************************************" << endl;
    cout << "**                   Student                   **" << endl;
    cout << "**                                             **" << endl;
    cout << "**                1. 成绩查询                  **" << endl;
    cout << "**                2.查看本班成绩               **" << endl;
    cout << "**                3. 成绩分析                  **" << endl;
    cout << "**                4.返回上一级                 **" << endl;
    cout << "**                                             **" << endl;
    cout << "*************************************************" << endl;
    cout << "*************************************************" << endl;
}
//////////////////////////////////////////////////////////////////////////////////
void teacherMainscreen() {
    cout << "*************************************************" << endl;
    cout << "*************************************************" << endl;
    cout << "**                   Teacher                   **" << endl;
    cout << "**                                             **" << endl;
    cout << "**           1.  增删改查学生信息              **" << endl;
    cout << "**           2.    查看班内成绩                **" << endl;
    cout << "**           3.      成绩分析                  **" << endl;
    cout << "**           4. 学生信息下载至文件             **" << endl;
    cout << "**           5.     返回上一级                 **" << endl;
    cout << "**                                             **" << endl;
    cout << "*************************************************" << endl;
    cout << "*************************************************" << endl;
}
void teacherSonscreen() {
    cout << "*************************************************" << endl;
    cout << "*************************************************" << endl;
    cout << "**                   Teacher                   **" << endl;
    cout << "**                                             **" << endl;
    cout << "**              1. 增加学生信息                **" << endl;
    cout << "**              2. 删除学生信息                **" << endl;
    cout << "**              3. 修改学生信息                **" << endl;
    cout << "**              4. 查找学生信息                **" << endl;
    cout << "**              5.  返回上一级                 **" << endl;
    cout << "**                                             **" << endl;
    cout << "*************************************************" << endl;
    cout << "*************************************************" << endl;
}
///////////////////////////////////////////////////////////////////////////////////
void managerMainscreen() {
    cout << "*************************************************" << endl;
    cout << "*************************************************" << endl;
    cout << "**                   Manager                   **" << endl;
    cout << "**                                             **" << endl;
    cout << "**           1.      查看待办                  **" << endl;
    cout << "**           2.  增删改查账密及信息            **" << endl;
    cout << "**           3.    从文件录入账密              **" << endl;
    cout << "**           4.    从文件导出账密              **" << endl;
    cout << "**           5.     返回上一级                 **" << endl;
    cout << "**                                             **" << endl;
    cout << "*************************************************" << endl;
    cout << "*************************************************" << endl;
}

// 新增：管理员待办子菜单
void adminTodoScreen() {
    cout << "*************************************************" << endl;
    cout << "*************************************************" << endl;
    cout << "**                   待办事项                  **" << endl;
    cout << "**                                             **" << endl;
    cout << "**         1. 查看系统统计信息                 **" << endl;
    cout << "**         2. 审批密码找回申请                 **" << endl;
    cout << "**         3. 返回上一级                       **" << endl;
    cout << "**                                             **" << endl;
    cout << "*************************************************" << endl;
    cout << "*************************************************" << endl;
}

int main() {
    // ========== 所有变量在开头定义 ==========
    int op = 0, op1 = 0, op2 = 0, op3 = 0, ops = 0, opt = 0, opm = 0, opzscg = 0, optodo = 0;
    char inID[20];
    char inpassWord[20];
    char ch;

    // ========== 初始化 ==========
    loadStudentsToLinkList();

    // ========== 主菜单 ==========
first:
    welcomeScreen();
    cout << "请输入您所需的操作：";
    cin >> op;
    system("cls");

    switch (op) {
    case 1: // 学生端
    a:
        loginscreen();
        cout << "请输入您所需的操作：";
        cin >> op1;
        switch (op1) {
        case 1: // 直接登录
            cout << "请输入账号：";
            cin >> inID;
            inputPassword(inpassWord, 20);
            if (studentLogin(inID, inpassWord)) {
                Sleep(2100);
                system("cls");
                goto s;
            }
            else {
                Sleep(2100);
                system("cls");
                goto a;
            }
            break;
        case 2: // 密码找回 - 提交申请
            submitStudentPwdRequest();
            Sleep(5000);
            system("cls");
            goto a;
            break;
        case 3: // 密码修改
            printf("请输入学号：");
            scanf("%s", inID);
            printf("请输入原密码：");
            inputPassword(inpassWord, 20);
            changeStudentPwd(inID, inpassWord);
            Sleep(2100);
            system("cls");
            goto a;
            break;
        case 4: // 密码注册
            studentRegister();
            Sleep(2100);
            system("cls");
            goto a;
            break;
        case 5: // 返回上一级
            system("cls");
            goto first;
            break;
        default:
            printf("无效选择！\n");
            Sleep(1000);
            system("cls");
            goto a;
        }
        break;

    case 2: // 教师端
    b:
        loginscreen();
        cout << "请输入您所需的操作：";
        cin >> op2;
        switch (op2) {
        case 1: // 直接登录
            cout << "请输入账号：";
            cin >> inID;
            inputPassword(inpassWord, 20);
            if (teacherLogin(inID, inpassWord)) {
                Sleep(2100);
                system("cls");
                goto t;
            }
            else {
                Sleep(2100);
                system("cls");
                goto b;
            }
            break;
        case 2: // 密码找回 - 提交申请
            submitTeacherPwdRequest();
            Sleep(3000);
            system("cls");
            goto b;
            break;
        case 3: // 密码修改
            printf("请输入工号：");
            scanf("%s", inID);
            printf("请输入原密码：");
            inputPassword(inpassWord, 20);
            changeTeacherPwd(inID, inpassWord);
            Sleep(2100);
            system("cls");
            goto b;
            break;
        case 4: // 密码注册
            teacherRegister();
            Sleep(2100);
            system("cls");
            goto b;
            break;
        case 5: // 返回上一级
            system("cls");
            goto first;
            break;
        default:
            printf("无效选择！\n");
            Sleep(1000);
            system("cls");
            goto b;
        }
        break;

    case 3: // 管理员端
    c:
        loginscreen();
        cout << "请输入您所需的操作：";
        cin >> op3;
        switch (op3) {
        case 1: // 直接登录
            cout << "请输入管理员工号：";
            cin >> inID;
            inputPassword(inpassWord, 20);
            if (adminLogin(inID, inpassWord)) {
                Sleep(2100);
                system("cls");
                goto m;
            }
            else {
                Sleep(2100);
                system("cls");
                goto c;
            }
            break;
        case 2: // 密码找回 - 管理员不需要
            printf("管理员账号请联系超级管理员重置密码！\n");
            Sleep(2000);
            system("cls");
            goto c;
            break;
        case 3: // 密码修改
            printf("请输入工号：");
            scanf("%s", inID);
            printf("请输入原密码：");
            inputPassword(inpassWord, 20);
            changeAdminPwd(inID, inpassWord);
            Sleep(2100);
            system("cls");
            goto c;
            break;
        case 4: // 密码注册
            adminRegister();
            Sleep(2100);
            system("cls");
            goto c;
            break;
        case 5: // 返回上一级
            system("cls");
            goto first;
            break;
        default:
            printf("无效选择！\n");
            Sleep(1000);
            system("cls");
            goto c;
        }
        break;

    default:
        printf("无效选择！\n");
        Sleep(1000);
        system("cls");
        goto first;
    }

    // ========== 学生主界面 ==========
s:
    studentMainscreen();
    cout << "请输入您所需的操作：";
    cin >> ops;
    switch (ops) {
    case 1: // 查询个人成绩
        queryMyScore();
        cout << endl << "按ESC退出...";
        ch = _getch();
        if (ch == 27) {
            system("cls");
            goto s;
        }
        break;
    case 2: // 查询班级成绩
        queryClassScore(loginStudent.classname);
        cout << endl << "按ESC退出...";
        ch = _getch();
        if (ch == 27) {
            system("cls");
            goto s;
        }
        break;
    case 3: // 成绩分析
        analyzeMyScore();
        cout << endl << "按ESC退出...";
        ch = _getch();
        if (ch == 27) {
            system("cls");
            goto s;
        }
        break;
    case 4: // 返回上一级
        system("cls");
        goto a;
        break;
    default:
        printf("无效选择！\n");
        Sleep(1000);
        system("cls");
        goto s;
    }
    return 0;

    // ========== 教师主界面 ==========
t:
    teacherMainscreen();
    cout << "请输入您所需的操作：";
    cin >> opt;
    switch (opt) {
    case 1: // 增删改查学生信息
    t1:
        system("cls");
        teacherSonscreen();
        cout << "请输入您所需的操作：";
        cin >> opzscg;
        switch (opzscg) {
        case 1:
            teacherAddStudent();
            cout << endl << "按ESC退出...";
            ch = _getch();
            if (ch == 27) {
                system("cls");
                goto t1;
            }
            break;
        case 2:
            teacherDeleteStudent();
            cout << endl << "按ESC退出...";
            ch = _getch();
            if (ch == 27) {
                system("cls");
                goto t1;
            }
            break;
        case 3:
            teacherModifyStudentScore();
            cout << endl << "按ESC退出...";
            ch = _getch();
            if (ch == 27) {
                system("cls");
                goto t1;
            }
            break;
        case 4:
            teacherQueryStudent();
            cout << endl << "按ESC退出...";
            ch = _getch();
            if (ch == 27) {
                system("cls");
                goto t1;
            }
            break;
        case 5:
            system("cls");
            goto t;
            break;
        default:
            printf("无效选择！\n");
            Sleep(1000);
            system("cls");
            goto t1;
        }
        break;
    case 2: // 查看班内成绩
        queryClassScore(loginTeacherClass);
        cout << endl << "按ESC退出...";
        ch = _getch();
        if (ch == 27) {
            system("cls");
            goto t;
        }
        break;
    case 3: // 成绩分析
        system("cls");
        teacherMainscreen();
        teacherAnalyzeScore();
        cout << endl << "按ESC退出...";
        ch = _getch();
        if (ch == 27) {
            system("cls");
            goto t;
        }
        break;
    case 4: // 学生信息下载
        teacherDownloadStudentInfo();
        cout << endl << "按ESC退出...";
        ch = _getch();
        if (ch == 27) {
            system("cls");
            goto t;
        }
        break;
    case 5: // 返回上一级
        system("cls");
        goto b;
        break;
    default:
        printf("无效选择！\n");
        Sleep(1000);
        system("cls");
        goto t;
    }
    return 0;

    // ========== 管理员主界面 ==========
m:
    managerMainscreen();
    cout << "请输入您所需的操作：";
    cin >> opm;
    switch (opm) {
    case 1: // 查看待办 - 新增子菜单
    mtodo:
        system("cls");
        adminTodoScreen();
        cout << "请输入您所需的操作：";
        cin >> optodo;
        switch (optodo) {
        case 1: // 查看统计信息
            adminViewDashboard();
            cout << endl << "按任意键返回...";
            _getch();
            system("cls");
            goto mtodo;
            break;
        case 2: // 审批密码找回申请
            adminProcessPwdRequests();
            cout << endl << "按任意键返回...";
            _getch();
            system("cls");
            goto mtodo;
            break;
        case 3: // 返回上一级
            system("cls");
            goto m;
            break;
        default:
            printf("无效选择！\n");
            Sleep(1000);
            system("cls");
            goto mtodo;
        }
        break;
    case 2: // 增删改查
        adminManageAccounts();
        system("cls");
        goto m;
        break;
    case 3: // 导入
        adminImportFromFile();
        cout << endl << "按任意键返回...";
        _getch();
        system("cls");
        goto m;
        break;
    case 4: // 导出
        adminExportToFile();
        cout << endl << "按任意键返回...";
        _getch();
        system("cls");
        goto m;
        break;
    case 5: // 返回上一级
        system("cls");
        goto c;
        break;
    default:
        printf("无效选择！\n");
        Sleep(1000);
        system("cls");
        goto m;
    }
    return 0;
}