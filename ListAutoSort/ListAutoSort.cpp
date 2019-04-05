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
	connect(ui.titileList, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotListItemMenu(QPoint)));
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
	QMessageBox::information(this, QStringLiteral("����"), str);
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
		titles.append(TitleItem(n, p));
	}
}

/**
 * ���˸���֮��ˢ���б�
 */
void ListAutoSort::refreshList()
{
	ui.titileList->clear();

	for (int i = 0; i < titles.size(); i++)
	{
		QListWidgetItem* item = new QListWidgetItem(titles[i].getName(), ui.titileList);
	}
}

/**
 * TableVIew���һ�У��������Ա༭
 */
void ListAutoSort::slotAddCol()
{
	QListWidgetItem* item = new QListWidgetItem(QStringLiteral("���ֶ�"), ui.titileList);
	titles.append(TitleItem(QStringLiteral("���ֶ�"), ""));
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

void ListAutoSort::slotListItemMenu(QPoint p)
{
	QListWidgetItem* item = ui.titileList->itemAt(p);
	if (item == NULL)
		return;

	QMenu* menu = new QMenu(this);
	QAction* deleteAction = new QAction(QStringLiteral("ɾ��"), this);
	menu->addAction(deleteAction);
	connect(deleteAction, SIGNAL(triggered()), this, SLOT(slotDeleteListItem()));
	menu->exec(QCursor::pos());
}

void ListAutoSort::slotDeleteListItem()
{
	QListWidgetItem* item = ui.titileList->currentItem();
	if (item == NULL)
		return;

	int index = ui.titileList->currentIndex().row();
	if (index >= 0 && index < titles.size())
		titles.removeAt(index);

	ui.titileList->takeItem(index); // ��֪��ΪʲôremoveWidgetItem��Ч
	delete item; // takeItem ��Ҫ�ֶ� delete
}
