#include "qt.pc.h"
#include "MainWindow.h"

int main(int argc, char *argv[])
{
	QApplication app{ argc, argv };
	app.setFont(QFont{ "Bahnschrift", 11 });

	std::unique_ptr<QMainWindow> mainWindow{};
	try
	{
		mainWindow = std::make_unique<MainWindow>();
		mainWindow->show();
	}
	catch (std::exception const& exception)
	{
		QMessageBox::critical(nullptr, "Error", exception.what());
	}

	return app.exec();
}
