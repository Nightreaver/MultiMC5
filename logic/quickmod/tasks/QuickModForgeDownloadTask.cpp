/* Copyright 2013 MultiMC Contributors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "QuickModForgeDownloadTask.h"

#include "logic/ForgeInstaller.h"
#include "logic/lists/ForgeVersionList.h"
#include "logic/OneSixInstance.h"
#include "logic/quickmod/QuickModsList.h"
#include "logic/quickmod/QuickMod.h"
#include "MultiMC.h"

#include "gui/dialogs/VersionSelectDialog.h"

QuickModForgeDownloadTask::QuickModForgeDownloadTask(InstancePtr instance, QObject *parent)
	: Task(parent), m_instance(instance)
{
}

void QuickModForgeDownloadTask::executeTask()
{
	auto mods =
		std::dynamic_pointer_cast<OneSixInstance>(m_instance)->getFullVersion()->quickmods;
	if (mods.isEmpty())
	{
		emitSucceeded();
		return;
	}

	ForgeInstaller *installer = new ForgeInstaller;
	if (QDir(m_instance->instanceRoot()).exists("patches/" + installer->id() + ".json"))
	{
		emitSucceeded();
		return;
	}

	QStringList versionFilters;
	for (auto it = mods.cbegin(); it != mods.cend(); ++it)
	{
		QuickModVersionPtr version = MMC->quickmodslist()->modVersion(it.key(), it.value());
		if (!version)
		{
			continue;
		}
		if (!version->forgeVersionFilter.isEmpty())
		{
			versionFilters.append(version->forgeVersionFilter);
		}
	}

	VersionSelectDialog vselect(MMC->forgelist().get(), tr("Select Forge version"));
	if (versionFilters.isEmpty())
	{
		vselect.setFilter(1, m_instance->currentVersionId());
	}
	else
	{
		vselect.setFilter(0, versionFilters.join(','));
	}
	vselect.setEmptyString(tr("No Forge versions are currently available for Minecraft ") +
						   m_instance->currentVersionId());
	if (vselect.exec() && vselect.selectedVersion())
	{
		auto task = installer->createInstallTask(
			std::dynamic_pointer_cast<OneSixInstance>(m_instance).get(),
			vselect.selectedVersion(), this);
		connect(task, &Task::progress, [this](qint64 current, qint64 total)
		{ setProgress(100 * current / qMax((qint64)1, total)); });
		connect(task, &Task::status, [this](const QString &msg)
		{ setStatus(msg); });
		connect(task, &Task::failed, [this, installer](const QString &reason)
		{
			delete installer;
			emitFailed(reason);
		});
		connect(task, &Task::succeeded, [this, installer]()
		{
			delete installer;
			emitSucceeded();
		});
		task->start();
	}
	else
	{
		emitFailed(tr("No forge version selected"));
	}
}