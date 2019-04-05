/**
 * �ı���������
 */

#ifndef STRINGUTIL_H
#define STRINGUTIL_H

#include <QString>
#include <QStringList>
#include <QColor>

QStringList getStrMids(QString text, QString l, QString r); // ȡ�м��ı�����
QString getStrMid(QString text, QString l, QString r); // ȡ�м��ı�2

QString fnEncode(QString text); // ��ʹ�õ��ļ�������
QString fnDecode(QString text); // �ļ�������

bool canRegExp(QString str, QString pat); // �ܷ�����ƥ��

QString getXml(QString str, QString pat);
QStringList getXmls(QString str, QString pat);
QString makeXml(QString str, QString pat);
QString makeXml(int i, QString pat);

QString ArabToCN(int num);

#endif // STRINGUTIL_H
