#define STR(ctx) QString::fromLocal8Bit(ctx).toUtf8().data()
#include "Widget.h"
#include "sWidget.h"

int main(int argc, char* argv[]) {  // 命令行参数的个数, 命令行的参数
	QApplication a(argc, argv);  // 创建应用

	Widget w;
	w.resize(421 * 2, 583 * 2);
	w.show();

	sWidget paint_widget(&w);
	paint_widget.setAttribute(Qt::WA_TransparentForMouseEvents);
	paint_widget.setFocusPolicy(Qt::NoFocus);
	paint_widget.resize(421, 583);
	paint_widget.show();

	return a.exec();  // 死循环
}