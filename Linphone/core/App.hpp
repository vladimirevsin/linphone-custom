/*
 * Copyright (c) 2010-2024 Belledonne Communications SARL.
 *
 * This file is part of linphone-desktop
 * (see https://www.linphone.org).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <QCommandLineParser>
#include <QQmlApplicationEngine>
#include <QSharedPointer>

#include "core/setting/SettingsCore.hpp"
#include "core/singleapplication/singleapplication.h"
#include "model/core/CoreModel.hpp"
#include "tool/AbstractObject.hpp"

class CallGui;
class Thread;
class Notifier;
class QQuickWindow;

class App : public SingleApplication, public AbstractObject {
public:
	App(int &argc, char *argv[]);
	~App();
	void setSelf(QSharedPointer<App>(me));
	static App *getInstance();
	Notifier *getNotifier() const;

	// App::postModelAsync(<lambda>) => run lambda in model thread and continue.
	// App::postModelSync(<lambda>) => run lambda in current thread and block connection.
	template <typename Func, typename... Args>
	static auto postModelAsync(Func &&callable, Args &&...args) {
		QMetaObject::invokeMethod(CoreModel::getInstance().get(), callable, args...);
	}
	template <typename Func>
	static auto postModelAsync(Func &&callable) {
		QMetaObject::invokeMethod(CoreModel::getInstance().get(), callable);
	}
	template <typename Func, typename... Args>
	static auto postCoreAsync(Func &&callable, Args &&...args) {
		QMetaObject::invokeMethod(App::getInstance(), callable, args...);
	}
	template <typename Func>
	static auto postCoreAsync(Func &&callable) {
		QMetaObject::invokeMethod(App::getInstance(), callable);
	}
	template <typename Func, typename... Args>
	static auto postCoreSync(Func &&callable, Args &&...args) {
		if (QThread::currentThread() == CoreModel::getInstance()->thread()) {
			bool end = false;
			postCoreAsync([&end, callable, args...]() mutable {
				QMetaObject::invokeMethod(App::getInstance(), callable, args..., Qt::DirectConnection);
				end = true;
			});
			while (!end)
				qApp->processEvents();
		} else {
			QMetaObject::invokeMethod(App::getInstance(), callable, Qt::DirectConnection);
		}
	}
	template <typename Func, typename... Args>
	static auto postModelSync(Func &&callable, Args &&...args) {
		if (QThread::currentThread() != CoreModel::getInstance()->thread()) {
			bool end = false;
			postModelAsync([&end, callable, args...]() mutable {
				QMetaObject::invokeMethod(CoreModel::getInstance().get(), callable, args..., Qt::DirectConnection);
				end = true;
			});
			while (!end)
				qApp->processEvents();
		} else {
			QMetaObject::invokeMethod(CoreModel::getInstance().get(), callable, Qt::DirectConnection);
		}
	}

	template <typename Func, typename... Args>
	static auto postModelBlock(Func &&callable, Args &&...args) {
		if (QThread::currentThread() != CoreModel::getInstance()->thread()) {
			QMetaObject::invokeMethod(CoreModel::getInstance().get(), callable, args..., Qt::BlockingQueuedConnection);
		} else {
			QMetaObject::invokeMethod(CoreModel::getInstance().get(), callable, Qt::DirectConnection);
		}
	}

	void clean();
	void init();
	void initCppInterfaces();

	void onLoggerInitialized();

	QQuickWindow *getCallsWindow(QVariant callGui = QVariant());
	void setCallsWindowProperty(const char *id, QVariant property);
	void closeCallsWindow();

	QQuickWindow *getMainWindow();

	QQmlApplicationEngine *mEngine = nullptr;
	bool notify(QObject *receiver, QEvent *event) override;

	enum class StatusCode { gRestartCode = 1000, gDeleteDataCode = 1001 };

private:
	void createCommandParser();

	QCommandLineParser *mParser = nullptr;
	Thread *mLinphoneThread = nullptr;
	Notifier *mNotifier = nullptr;
	QQuickWindow *mMainWindow = nullptr;
	QQuickWindow *mCallsWindow = nullptr;
	QSharedPointer<Settings> mSettings;
	QSharedPointer<SafeConnection<App, CoreModel>> mCoreModelConnection;

	DECLARE_ABSTRACT_OBJECT
};
