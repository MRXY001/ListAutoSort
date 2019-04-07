#include "TableAITool.h"

TableAITool::TableAITool(QString m, QList<FieldItem>f) : mixture(m), fields(f)
{
}

void TableAITool::start()
{
	init();

	filter();

	compareFields();
	compareRegExp();
	compareMissing();
}

/**
 * ��ʼ������
 */
void TableAITool::init()
{
	for (int i = 0; i < count; i++)
	{
		result << "";          // �ȳ�ʼ���ɿյ�
		capture.append(false);
	}
}

/**
 * �滻�ı��е� ���š�ð�š��ٺ�
 */
void TableAITool::filter()
{
	mixture = mixture.replace(QRegExp(QStringLiteral("[:,��������\\s]+")), "\n");
	//QMessageBox::information(NULL, "replace", mixture);
	infos = mixture.split("\n", QString::SplitBehavior::SkipEmptyParts);
}

/**
 * �Ƚϣ��ֶ���һģһ����
 */
void TableAITool::compareFields()
{
	for (int i = 0; i < fields.size(); i++)
	{
		for (int j = 0; j < infos.size(); j++)
		{
			if (fields[i].getName() == infos[j])
			{

			}
		}
	}
}

/**
 * �Ƚϣ�������ʽƥ���
 */
void TableAITool::compareRegExp()
{

}

/*
 *�Ƚϣ�ʣ��û��ƥ���*/
void TableAITool::compareMissing()
{

}

QStringList TableAITool::getResult()
{
	return result;
}