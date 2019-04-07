#include "TableAITool.h"

TableAITool::TableAITool(QString m, QList<FieldItem>f) : mixture(m), fields(f) // ��QList��������������£�
{
	start();
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
	for (int i = 0; i < fields.size(); i++)
	{
		result << "";          // �ȳ�ʼ���ɿյ�
		captured.append(false);
	}
}

/**
 * �滻�ı��е� ���š�ð�š��ٺ�
 */
void TableAITool::filter()
{
	mixture = mixture.replace(QRegExp(QStringLiteral("[:,��������\\s]+")), "\n");
	//QMessageBox::information(NULL, "replace", mixture);
	qDebug() << QStringLiteral("�滻����ı���") << mixture;
	infos = mixture.split("\n", QString::SplitBehavior::SkipEmptyParts);
	qDebug() << QStringLiteral("�ָ��ĳ��ȣ�") << infos.size();
}

/**
 * �Ƚϣ��ֶ���һģһ����
 */
void TableAITool::compareFields()
{
	QList<int>same; // same �� infos �ȳ�
	for (int i = 0; i < infos.size(); i++)
		same << -1;

	// �Ƚ�ÿ���ֶκ�ÿ����Ϣ���ַ���
	for (int i = 0; i < fields.size(); i++) // �����ֶ�
	{
		for (int j = 0; j < infos.size(); j++) // ������Ϣ
		{
			if (fields[i].getName() == infos[j])
			{
				same[j] = i;
			}
		}
	}

	// ��һ��һ��������һ����һ�������������ֶδ��ң�
	for (int i = 0; i < infos.size()-1; i++)
	{
		// ����ֶ�ƥ���ˣ����� i+1��ɾ�� i �Լ� i+1
		if (same[i] > -1 && same[i + 1] == -1)
		{
			int index = same[i]; // ��Ӧ�ֶε�����
			qDebug() << QStringLiteral("ƥ�䣺") << fields[index].getName() << "  " << infos[i + 1];
			if (captured[index]) // �ظ�ƥ���ˡ���
				continue;
			result[index] = infos[i + 1];
			captured[index] = true;
		}
	}

	// ɾ���Ѿ�ƥ��ģ�����ɾ����
	for (int i = infos.size() - 1; i >= 0; i--)
	{
		// ɾ��������
		if (same[i] > -1 && same[i + 1] == -1)
		{
			infos.removeAt(i + 1);
			infos.removeAt(i);
		}
	}
}

/**
 * �Ƚϣ�������ʽƥ���
 */
void TableAITool::compareRegExp()
{
	QList<IndexConnection>ics;

	// ������е���ƥ���
	bool find = true;
	int index = 0;
	while (find)
	{
		if (index >= fields.size())
			break;
		if (captured[index] || fields[index].getPattern().isEmpty())
		{
			index++;
			continue;
		}

		// Ѱ��������ƥ�����Ϣ���Լ���һ��ƥ���λ��
		int count = 0, place = -1;
		for (int i = 0; i < infos.size(); i++)
			if (canMatch(infos[i], fields[index].getPattern()))
			{
				count++;
				if (place == -1)
					place = i;
			}

		// ���ҽ�����һ����ƥ��
		if (count == 1)
		{
			result[index] = infos[place];
			qDebug() << QStringLiteral("����ƥ�䵽��") << fields[index].getName() << "    " << infos[place] << "    " << fields[index].getPattern();
			infos.removeAt(place);  // ƥ���ɾ��
			captured[index] = true; // ����ֶ�ƥ����
			index = 0;              // ��ͷ��ʼ����
		}
		else
		{
			index++;                // û��ƥ�䣬����������һ��
		}
	}
}

/*
 *�Ƚϣ�ʣ��û��ƥ���*/
void TableAITool::compareMissing()
{

}

bool TableAITool::canMatch(QString str, QString reg)
{
	return QRegExp(reg).indexIn(str) > -1;
}

QStringList TableAITool::getResult()
{
	return result;
}

QString TableAITool::getResultString()
{
	QString msg = "";
	for (int i = 0; i < result.size(); i++)
	{
		if (i > 0) msg += ", ";
		msg += result[i];
	}
	return msg;
}
