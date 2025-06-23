#include "aupch.h"
#include "ScriptEngine.h"

//#include "FileWatch.hpp"
#include "ScriptGlue.h"

namespace Aurora {

	struct ScriptEngineData
	{
		// MonoDomain* RootDomain = nullptr;
		// MonoDomain* AppDomain = nullptr;

		// MonoAssembly* CoreAssembly = nullptr;
		// MonoImage* CoreAssemblyImage = nullptr;
		
		// MonoAssembly* AppAssembly = nullptr;
		// MonoImage* AppAssemblyImage = nullptr;

		std::filesystem::path CoreAssemblyFilepath;
		std::filesystem::path AppAssemblyFilepath;

		ScriptClass EntityClass;

		std::unordered_map<std::string, std::shared_ptr<ScriptClass>> EntityClasses;
		std::unordered_map<UUID, std::shared_ptr<ScriptInstance>> EntityInstances;
		std::unordered_map<UUID, ScriptFieldMap> EntityScriptFields;

		// std::unique_ptr<filewatch::FileWatch<std::string>> AppAssemblyFileWatcher;
		bool AssemblyReloadPending = false;

#ifdef AU_DEBUG
		bool EnableDebugging = true;
#else
		bool EnableDebugging = false;
#endif

		Scene* SceneContext = nullptr;
	};

	static ScriptEngineData* s_Data = nullptr;
	
	ScriptClass::ScriptClass(const std::string& classNamespace, const std::string& className, bool isCore) {
	}

	ScriptInstance::ScriptInstance(std::shared_ptr<ScriptClass> scriptClass, Entity entity) {
	}

	void ScriptInstance::InvokeOnCreate() {
		// if (m_OnCreateMethod)
		// 	m_ScriptClass->InvokeMethod(m_Instance, m_OnCreateMethod);
	}

	void ScriptInstance::InvokeOnUpdate(float ts) {
		// if (m_OnUpdateMethod) {
		// 	void* param = &ts;
		// 	m_ScriptClass->InvokeMethod(m_Instance, m_OnUpdateMethod, &param);
		// }
	}

	bool ScriptInstance::GetFieldValueInternal(const std::string& name, void* buffer) {
		return false;
	}

	bool ScriptInstance::SetFieldValueInternal(const std::string& name, const void* value) {
		return false;
	}

	void ScriptEngine::Init() {
		s_Data = new ScriptEngineData();
		InitMono();
		ScriptGlue::RegisterFunctions();

		LoadAssemblyClasses();

		ScriptGlue::RegisterComponents();
		
		s_Data->EntityClass = ScriptClass("Aurora", "Entity", true);
	}

	void ScriptEngine::Shutdown() {
		delete s_Data;
		s_Data = nullptr;
	}

	void ScriptEngine::OnRuntimeStart(Scene* scene) {
	}

	void ScriptEngine::OnRuntimeStop() {
	}

	bool ScriptEngine::EntityClassExists(const std::string& fullClassName) {
		return false; // TODO
	}

	Scene* ScriptEngine::GetSceneContext() {
		return nullptr; // TODO
	}

	std::shared_ptr<ScriptInstance> ScriptEngine::GetEntityScriptInstance(UUID entityID) {
		return nullptr; // TODO
	}

	std::shared_ptr<ScriptClass> ScriptEngine::GetEntityClass(const std::string& name) {
		return nullptr; // TODO
	}

	std::unordered_map<std::string, std::shared_ptr<ScriptClass>> ScriptEngine::GetEntityClasses() {
		return s_Data->EntityClasses;
	}

	ScriptFieldMap& ScriptEngine::GetScriptFieldMap(Entity entity) {
		UUID entityID = entity.GetUUID();
		return s_Data->EntityScriptFields[entityID];
	}

	void ScriptEngine::InitMono() {
	}

	void ScriptEngine::ShutdownMono() {
	}

	void ScriptEngine::LoadAssemblyClasses() {
	}

	void ScriptEngine::OnCreateEntity(Entity entity) {
		const auto& sc = entity.GetComponent<ScriptComponent>();
		if (ScriptEngine::EntityClassExists(sc.ClassName)) {
			UUID entityID = entity.GetUUID();

			std::shared_ptr<ScriptInstance> instance = std::make_shared<ScriptInstance>(s_Data->EntityClasses[sc.ClassName], entity);
			s_Data->EntityInstances[entityID] = instance;

			// Copy field values
			if (s_Data->EntityScriptFields.find(entityID) != s_Data->EntityScriptFields.end()) {
				const ScriptFieldMap& fieldMap = s_Data->EntityScriptFields.at(entityID);
				for (const auto& [name, fieldInstance] : fieldMap)
					instance->SetFieldValueInternal(name, fieldInstance.m_Buffer);
			}

			instance->InvokeOnCreate();
		}
	}
	
	void ScriptEngine::OnUpdateEntity(Entity entity, Timestep ts) {
		UUID entityUUID = entity.GetUUID();
		if (s_Data->EntityInstances.find(entityUUID) != s_Data->EntityInstances.end()) {
			std::shared_ptr<ScriptInstance> instance = s_Data->EntityInstances[entityUUID];
			instance->InvokeOnUpdate((float)ts);
		} else {
			AU_CORE_ERROR("Could not find ScriptInstance for entity {}",  static_cast <uint32_t>(entityUUID));
		}
	}


}
