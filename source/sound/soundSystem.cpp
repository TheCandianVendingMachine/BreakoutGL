#include "soundSystem.hpp"
#include <fmod_errors.h>
#include <spdlog/spdlog.h>
#include <exception>

soundSystem *globals::g_audioSystem = nullptr;

void soundSystem::verifyImportant(FMOD_RESULT result) const
	{
		if (result != FMOD_OK)
			{
				spdlog::error("FMOD error: ({}) {}", result, FMOD_ErrorString(result));
				std::terminate();
			}
	}

bool soundSystem::verifyCasual(FMOD_RESULT result) const
	{
		if (result != FMOD_OK)
			{
				spdlog::error("FMOD error: ({}) {}", result, FMOD_ErrorString(result));
				return false;
			}
		return true;
	}

void soundSystem::loadBank(const char* bankPath)
	{
		verifyImportant(m_system->loadBankFile(bankPath, FMOD_STUDIO_LOAD_BANK_NORMAL, &m_loadedBanks[++m_bankID]));

		FMOD::Studio::Bank *bank = m_loadedBanks.at(m_bankID);

		int eventCount = 0;
		verifyImportant(bank->getEventCount(&eventCount));

		if (eventCount > 0)
			{
				std::vector<FMOD::Studio::EventDescription*> descriptions(eventCount);
				verifyImportant(bank->getEventList(descriptions.data(), eventCount, &eventCount));

				FMOD_GUID guid;
				for (auto &description : descriptions)
					{
						if (verifyCasual(description->getID(&guid)))
							{
								fe::str guidStr = getStrFromGUID(&guid);
								m_descriptions[guidStr] = description;
							}
					}
			}
	}

FMOD::Studio::EventInstance *soundSystem::createInstance(const char *event)
	{
		FMOD::Studio::EventInstance *instance = nullptr;
		FMOD_GUID guid;
		if (!verifyCasual(m_system->lookupID(event, &guid))) { return instance; }

		FMOD::Studio::EventDescription* description = m_descriptions.at(getStrFromGUID(&guid));
		if (verifyCasual(description->createInstance(&instance)))
			{
				verifyImportant(instance->start());

				m_playingInstances.insert(instance);
			}
		return instance;
	}

soundSystem::soundSystem()
	{
		FMOD_STUDIO_INITFLAGS studioFlags = FMOD_STUDIO_INIT_LIVEUPDATE;
		FMOD_INITFLAGS fmodFlags = FMOD_INIT_PROFILE_ENABLE;

		verifyImportant(FMOD::Studio::System::create(&m_system));
		verifyImportant(m_system->initialize(512, studioFlags, fmodFlags, nullptr));

		// Must load at least the master bank for game to function
		loadBank("Master.bank");
		loadBank("Master.strings.bank");
		loadBank("Game SFX.bank");
	}

soundSystem::~soundSystem()
	{
		for (auto &[id, bank] : m_loadedBanks)
			{
				verifyImportant(bank->unload());
			}

		verifyImportant(m_system->release());
	}

void soundSystem::update()
	{
		verifyImportant(m_system->update());

		FMOD_STUDIO_PLAYBACK_STATE state;
		for (auto it = m_playingInstances.begin(); it != m_playingInstances.end();)
			{
				FMOD::Studio::EventInstance *instance = *it;

				if (verifyCasual(instance->getPlaybackState(&state)) && state == FMOD_STUDIO_PLAYBACK_STOPPED)
					{
						verifyImportant(instance->release());
						it = m_playingInstances.erase(it);
					}
				else
					{
						++it;
					}
			}
	}

void soundSystem::play(const char* event)
	{
		createInstance(event);
	}
