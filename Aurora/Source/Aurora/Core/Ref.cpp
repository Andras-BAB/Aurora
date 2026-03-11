#include "aupch.h"
#include "Ref.h"

#include <mutex>

namespace Core {
	namespace RefUtils {
		static std::mutex s_LiveReferenceMutex;
		static std::unordered_set<void*> s_LiveReferences;

		void AddToLiveReferences(void* instance) {
			std::lock_guard<std::mutex> lock(s_LiveReferenceMutex);
			s_LiveReferences.insert(instance);
		}

		void RemoveFromLiveReference(void* instance) {
			std::lock_guard<std::mutex> lock(s_LiveReferenceMutex);
			s_LiveReferences.erase(instance);
		}

		bool IsLive(void* instance) {
			std::lock_guard<std::mutex> lock(s_LiveReferenceMutex);
			return s_LiveReferences.contains(instance);
		}
	}
}
