#ifndef TABLEAITOOL_H
#define TABLEAITOOL_H

#include <QStringList>
#include <QDebug>
#include <QRegExp>
#include <QMessageBox>
#include "FieldItem.h"

class TableAITool
{
	struct IndexConnection
	{
		int field; // �ֶ�����
		int info;  // ��Ϣ����
		int type;

		IndexConnection(int f, int i) : field(f), info(i) { }
		IndexConnection(int f, int i, int t) : field(f), info(i), type(t) { }
	};

public:
	TableAITool(QString m, QList<FieldItem>f);

	QStringList getResult();
	QString getResultString();

protected:
	void start();
	void init();
	void filter();
	void compareFields();
	void compareRegExp();
	void compareMissing();

private:
	bool canMatch(QString str, QString reg);

protected:
	QString mixture;        // ԭ�ı������˺�ģ�
	QList<FieldItem>fields; // �ֶ��б�����+���ʽ
	QStringList infos;      // �ָ���ÿ����Ϣ
	QList<bool>captured;     // �Ƿ��Ѿ�ƥ����
	QStringList result;     // ���ؽ��
};

#endif