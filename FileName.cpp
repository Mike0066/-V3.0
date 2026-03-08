//学生
//王 12345 123456
//李 111111 111111
//老师
//张 123456 123456
//管理员
//123456 123456
#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include <windows.h>
#include <conio.h>

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
//////////////////////////////////////////////////////////////////////////////////////
//隐藏式输入密码，输入结果存入pwd，max_len为密码最大长度
void inputPassword(char* pwd, int max_len)
{
    int i = 0;
    char ch;
    memset(pwd, 0, max_len);  // 清空密码数组
    printf("请输入密码：");
    while (1)
    {
        ch = _getch();  // 无回显获取按键
        if (ch == '\r' || ch == '\n')
        {  // 按回车结束输入
            break;
        }
        else if (ch == '\b' && i > 0)
        {  // 按退格键删除
            printf("\b \b");  // 删掉屏幕上的*
            i--;
        }
        else if (i < max_len - 1)
        {  // 正常输入字符
            pwd[i++] = ch;
            printf("%c", PWD_MASK);    // 显示*
        }
    }
    printf("\n");
}

//////////////////////////////////////////////////////////////////////////////////////////

typedef struct Student
{
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
};

// 学生登录验证：返回1表示登录成功，0失败，-1文件错误
int studentLogin(char* inputId, char* inputPwd)
{
    FILE* fp = fopen(STUDENT_FILE, "rb");
    if (fp == NULL)
    {
        printf("账号文件不存在或打开失败！\n");
        return -1;
    }
    Student stu;
    // 循环读取文件中的每个学生信息
    while (fread(&stu, sizeof(Student), 1, fp) == 1)
    {
        // 比对学号和密码
        if (strcmp(stu.id, inputId) == 0 && strcmp(stu.password, inputPwd) == 0)
        {
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
int studentRegister()
{
    Student newStu;
    FILE* fp = fopen(STUDENT_FILE, "ab+");  // 追加+二进制模式
    if (fp == NULL)
    {
        printf("文件打开失败！\n");
        return -1;
    }
    // 1. 输入账号（学号）并检查是否已存在
    printf("请输入学号（账号）：");
    scanf("%s", newStu.id);
    // 先检查账号是否重复
    Student temp;
    fseek(fp, 0, SEEK_SET);  // 回到文件开头
    while (fread(&temp, sizeof(Student), 1, fp) == 1)
    {
        if (strcmp(temp.id, newStu.id) == 0)
        {
            fclose(fp);
            printf("该学号已注册！\n");
            return 0;
        }
    }
    // 2. 输入其他信息
    printf("请输入姓名：");
    scanf("%s", newStu.name);
    // 3. 隐藏输入密码
    char pwd1[20], pwd2[20];
    inputPassword(pwd1, 20);
    printf("请再次输入密码：");
    inputPassword(pwd2, 20);
    if (strcmp(pwd1, pwd2) != 0)
    {
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
int changeStudentPwd(char* inputId, char* oldPwd)
{
    FILE* fp;
    errno_t err = fopen_s(&fp, STUDENT_FILE, "rb+");
    if (err != 0 || fp == NULL)
    {
        printf("文件打开失败！\n");
        return -1;
    }

    Student stu;
    int found = 0;
    while (fread(&stu, sizeof(Student), 1, fp) == 1)
    {
        if (strcmp(stu.id, inputId) == 0 && strcmp(stu.password, oldPwd) == 0)
        {
            found = 1;
            char newPwd1[20], newPwd2[20];
            printf("请输入新密码：");
            inputPassword(newPwd1, 20);
            printf("请再次输入新密码：");
            inputPassword(newPwd2, 20);
            if (strcmp(newPwd1, newPwd2) != 0)
            {
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


///////////////////////////////////////////////////////////////////////////////////////
// 教师账号文件路径

typedef struct Teacher
{
    char id[MAX_ID_LEN];           //工号
    char password[PASSWORD_LEN];   //密码
    char name[MAX_NAME_LEN];       //姓名
    char manageClass[MAX_CLASS_LEN]; //班级
    int age;                       //年龄
    char sex;                   //性别(M/F)
};

// 教师登录验证：返回1成功，0失败，-1文件错误
int teacherLogin(char* inputId, char* inputPwd)
{
    FILE* fp;
    errno_t err = fopen_s(&fp, TEACHER_FILE, "rb");
    if (err != 0 || fp == NULL)
    {
        printf("教师账号文件不存在或打开失败！\n");
        return -1;
    }
    Teacher tea;
    // 循环读取每个教师信息
    while (fread(&tea, sizeof(Teacher), 1, fp) == 1)
    {
        // 比对工号和密码
        if (strcmp(tea.id, inputId) == 0 && strcmp(tea.password, inputPwd) == 0)
        {
            fclose(fp);
            printf("登录成功！欢迎你，%s老师\n", tea.name);
            // 额外返回教师管理的班级信息（可选）
            printf("你管理的班级ID：%d\n", tea.manageClass);
            return 1;
        }
    }
    fclose(fp);
    printf("教师工号或密码错误！\n");
    return 0;
}

// 教师账号注册：返回1成功，0失败（账号已存在），-1文件错误
int teacherRegister()
{
    Teacher newTea;
    FILE* fp;
    errno_t err = fopen_s(&fp, TEACHER_FILE, "ab+");  // 追加+二进制模式
    if (err != 0 || fp == NULL)
    {
        printf("文件打开失败！\n");
        return -1;
    }

    // 1. 输入工号并检查是否重复
    printf("请输入教师工号（账号）：");
    scanf("%s", newTea.id);
    Teacher temp;
    fseek(fp, 0, SEEK_SET);  // 回到文件开头
    while (fread(&temp, sizeof(Teacher), 1, fp) == 1)
    {
        if (strcmp(temp.id, newTea.id) == 0)
        {
            fclose(fp);
            printf("该教师工号已注册！\n");
            return 0;
        }
    }

    // 2. 输入教师信息
    printf("请输入教师姓名：");
    scanf("%s", newTea.name);
    printf("请输入你管理的班级ID：");
    scanf("%d", &newTea.manageClass);

    // 3. 隐藏输入密码（两次确认）
    char pwd1[20], pwd2[20];
    inputPassword(pwd1, 20);
    printf("请再次输入密码：");
    inputPassword(pwd2, 20);
    if (strcmp(pwd1, pwd2) != 0)
    {
        fclose(fp);
        printf("两次密码输入不一致！\n");
        return 0;
    }
    strcpy(newTea.password, pwd1);

    // 4. 写入文件
    fwrite(&newTea, sizeof(Teacher), 1, fp);
    fclose(fp);
    printf("教师账号注册成功！\n");
    return 1;
}

// 修改教师密码：输入原工号和原密码，验证通过后修改
int changeTeacherPwd(char* inputId, char* oldPwd)
{
    FILE* fp;
    errno_t err = fopen_s(&fp, TEACHER_FILE, "rb+");  // 读写模式
    if (err != 0 || fp == NULL)
    {
        printf("文件打开失败！\n");
        return -1;
    }
    Teacher tea;
    int found = 0;
    while (fread(&tea, sizeof(Teacher), 1, fp) == 1)
    {
        if (strcmp(tea.id, inputId) == 0 && strcmp(tea.password, oldPwd) == 0)
        {
            found = 1;
            // 输入新密码
            char newPwd1[20], newPwd2[20];
            printf("请输入新密码：");
            inputPassword(newPwd1, 20);
            printf("请再次输入新密码：");
            inputPassword(newPwd2, 20);
            if (strcmp(newPwd1, newPwd2) != 0)
            {
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
///////////////////////////////////////////////////////////////////////////////////////////////

typedef struct Admin
{
    char id[MAX_ID_LEN];           //学号
    char password[PASSWORD_LEN];   //密码
    char name[MAX_NAME_LEN];       //姓名
};

// 管理员登录验证：返回1成功，0失败，-1文件错误
int adminLogin(char* inputId, char* inputPwd)
{
    FILE* fp;
    errno_t err = fopen_s(&fp, ADMIN_FILE, "rb");
    if (err != 0 || fp == NULL)
    {
        printf("管理员账号文件不存在或打开失败！\n");
        return -1;
    }
    Admin admin;
    while (fread(&admin, sizeof(Admin), 1, fp) == 1)
    {
        // 比对管理员工号和密码
        if (strcmp(admin.id, inputId) == 0 && strcmp(admin.password, inputPwd) == 0)
        {
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
int adminRegister()
{
    Admin newAdmin;
    FILE* fp;
    errno_t err = fopen_s(&fp, ADMIN_FILE, "ab+");
    if (err != 0 || fp == NULL)
    {
        printf("文件打开失败！\n");
        return -1;
    }
    // 1. 输入管理员工号并检查重复
    printf("请输入管理员工号（账号）：");
    scanf("%s", newAdmin.id);
    Admin temp;
    fseek(fp, 0, SEEK_SET);
    while (fread(&temp, sizeof(Admin), 1, fp) == 1)
    {
        if (strcmp(temp.id, newAdmin.id) == 0)
        {
            fclose(fp);
            printf("该管理员工号已注册！\n");
            return 0;
        }
    }
    // 2. 隐藏输入密码（两次确认）
    char pwd1[20], pwd2[20];
    inputPassword(pwd1, 20);
    printf("请再次输入密码：");
    inputPassword(pwd2, 20);
    if (strcmp(pwd1, pwd2) != 0)
    {
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
int changeAdminPwd(char* inputId, char* oldPwd)
{
    FILE* fp;
    errno_t err = fopen_s(&fp, ADMIN_FILE, "rb+");
    if (err != 0 || fp == NULL)
    {
        printf("文件打开失败！\n");
        return -1;
    }
    Admin admin;
    int found = 0;
    while (fread(&admin, sizeof(Admin), 1, fp) == 1)
    {
        if (strcmp(admin.id, inputId) == 0 && strcmp(admin.password, oldPwd) == 0)
        {
            found = 1;
            // 输入新密码
            char newPwd1[20], newPwd2[20];
            printf("请输入新密码：");
            inputPassword(newPwd1, 20);
            printf("请再次输入新密码：");
            inputPassword(newPwd2, 20);
            if (strcmp(newPwd1, newPwd2) != 0)
            {
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
    if (!found)
    {
        printf("管理员工号或原密码错误！\n");
        return 0;
    }
    printf("密码修改成功！\n");
    return 1;
}
/////////////////////////////////////////////////////////////////////////////////////////
void welcomeScreen()
{
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
void loginscreen()
{
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
void studentMainscreen()
{
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
void teacherMainscreen()
{
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
///////////////////////////////////////////////////////////////////////////////////
void managerMainscreen()
{
    cout << "*************************************************" << endl;
    cout << "*************************************************" << endl;
    cout << "**                   Manager                   **" << endl;
    cout << "**                                             **" << endl;
    cout << "**           1.      查看代办                  **" << endl;
    cout << "**           2.  增删改查账密及信息            **" << endl;
    cout << "**           3.    从文件录入账密              **" << endl;
    cout << "**           4.    从文件导出账密              **" << endl;
    cout << "**           5.     返回上一级                 **" << endl;
    cout << "**                                             **" << endl;
    cout << "*************************************************" << endl;
    cout << "*************************************************" << endl;
}

int main()
{
first:
    welcomeScreen();
    cout << "请输入您所需的操作：";
    int op;
    char inID[20];
    char inpassWord[20];
    cin >> op;
    system("cls");
    switch (op)
    {
    case 1://student端
        loginscreen();
        cout << "请输入您所需的操作：";
        int op1;
        cin >> op1;
        switch (op1)
        {
        case 1://直接登录
            cout << "请输入账号：";
            cin >> inID;
            inputPassword(inpassWord, 20);
            studentLogin(inID, inpassWord);
            break;
        case 2://密码找回
            break;
        case 3://密码修改
            printf("请输入学号：");
            scanf("%s", inID);
            printf("请输入原密码：");
            inputPassword(inpassWord, 20);
            changeStudentPwd(inID, inpassWord);
            break;
        case 4://密码注册
            studentRegister();
            break;
        case 5://返回上一级
            system("cls");
            goto first;
            break;
        }
        break;
    case 2://teacher端
        loginscreen();
        cout << "请输入您所需的操作：";
        int op2;
        cin >> op2;
        switch (op2)
        {
        case 1://直接登录
            cout << "请输入账号：";
            cin >> inID;
            inputPassword(inpassWord, 20);
            teacherLogin(inID, inpassWord);
            break;
        case 2://密码找回
            break;
        case 3://密码修改
            printf("请输入工号：");
            scanf("%s", inID);
            printf("请输入原密码：");
            inputPassword(inpassWord, 20);
            changeTeacherPwd(inID, inpassWord);
            break;
        case 4://密码注册
            teacherRegister();
            break;
        case 5://返回上一级
            system("cls");
            goto first;
            break;
        }
        break;
    case 3://manager端
        loginscreen();
        cout << "请输入您所需的操作：";
        int op3;
        cin >> op3;
        switch (op3)
        {
        case 1://直接登录
            cout << "请输入管理员工号：";
            cin >> inID;
            inputPassword(inpassWord, 20);
            adminLogin(inID, inpassWord);
            break;
        case 2://密码找回
            break;
        case 3://密码修改
            printf("请输入工号：");
            scanf("%s", inID);
            printf("请输入原密码：");
            inputPassword(inpassWord, 20);
            changeAdminPwd(inID, inpassWord);
            break;
        case 4://密码注册
            adminRegister();
            break;
        case 5://返回上一级
            system("cls");
            goto first;
            break;
        }
        break;
    }
    return 0;
}