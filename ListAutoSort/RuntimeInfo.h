/**
 * ȫ������״̬��
 */

#ifndef RUNTIMEINFO_H
#define RUNTIMEINFO_H

#include <QApplication>
#include <QWidget>
#include <QMessageBox>
#include <QObject>
#include "fileutil.h"
#include "defines.h"


class RuntimeInfo
{
public:
    RuntimeInfo();

    void initPath();

public:
    // ==== ·�� ====
    QString APP_PATH;   // ����·��
    QString DATA_PATH;  // ������·��

    // ==== ���� ====
    qint64 startup_timestamp; // ����ʱ��
    bool is_initing; // ��ʼ����
    int has_new_version;

    // ==== ͳ�� ====
    int USER_WORD; // �û�����
    int USER_TIME; // �û�ʱ��
    int USER_ADDI; // �������
    int USER_LEVEL; // �û��ȼ�

    // ==== �ʵ� ====
    int NO_NOVEL_PROMPT_COUNT;  // ��ʾ�����鼮������3������ʾ
    bool is_line_guide;
    int line_guide_case;
    QWidget* guide_widget;
};


#endif // RUNTIMEINFO_H
