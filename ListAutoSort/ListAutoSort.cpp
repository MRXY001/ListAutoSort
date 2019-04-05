#include "ListAutoSort.h"

ListAutoSort::ListAutoSort(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	initView();
	initTable();
}

/**
 * ��ʼ���ؼ�������
 */
void ListAutoSort::initView()
{
	connect(ui.addCol, SIGNAL(clicked()), this, SLOT(slotAddCol()));
	connect(ui.inputButton, SIGNAL(clicked()), this, SLOT(slotInputButtonClicked()));
	connect(ui.pasteButton, SIGNAL(clicked()), this, SLOT(slotPasteButtonClicked()));
}

/**
 * ��ʼ��������
 */
void ListAutoSort::initTable()
{

}

/**
 * ��һ�λ������ֱܷ治ͬ����Ϣ
 */
void ListAutoSort::smartAddInfo(QString str)
{

}

/*
 * �ӱ�����ļ��ж�ȡ
 */
void ListAutoSort::readInfoTitles()
{
	QString savedPath = rt->DATA_PATH+"titles.txt";
	QString content;

	for (int i = 0; i < titles.size(); i++)
	{
		QString n = titles[i].getName();
		QString p = titles[i].getPattern();
		QString c = toXml(n, "name") + toXml(p, "pattern");
		content += toXml(c, "title");
	}

	writeTextFile(savedPath, content);
}

/**
 * ���浽�ļ�֮��
 */
void ListAutoSort::writeInfoTitles()
{
	QString savedPath = rt->DATA_PATH + "titles.txt";
	QString content = readTextFile(savedPath);
	QStringList ts = getXmls(content, "titles");

	for (int i = 0; i < ts.size(); i++)
	{
		QString n = getXml(ts[i], "name");
		QString p = getXml(ts[i], "pattern");
		titles.append(InfoItem(n, p));
	}
}

/**
 * ���˸���֮��ˢ���б�
 */
void ListAutoSort::refreshList()
{

}

/**
 * TableVIew���һ�У��������Ա༭
 */
void ListAutoSort::slotAddCol()
{

}

/**
 * ��� ui.inputEdit �е����ݵ������
 */
void ListAutoSort::slotInputButtonClicked()
{
	QString str = ui.inputEdit->toPlainText();
	smartAddInfo(str);
}

/**
 * ֱ����Ӽ������е����ݵ������
 */
void ListAutoSort::slotPasteButtonClicked()
{
	const QClipboard* clipboard = QApplication::clipboard();
	QString str = clipboard->text();
	smartAddInfo(str);
}
