#include "ServerApplicationTree.h"

namespace HttpServer
{
	ServerApplicationsTree::ServerApplicationsTree() noexcept:(app_sets(nullptr))
	{
	}

	ServerApplicationsTree::~ServerApplicationsTree() noexcept
	{
		this->clear();
	}	

		bool ServerApplicationsTree::empty() const noexcept
		{
		
		}

		void ServerApplicationsTree::addApplication(const std::string &name, ServerApplicationSettings *sets);
		void ServerApplicationsTree::addApplication(std::vector<std::string> &nameParts, ServerApplicationSettings *sets)
		{
		
		}

		const ServerApplicationsTree::ServerApplicationSettings *find(const std::string &name) const
		{
		
		}

		const SServerApplicationsTree::ServerApplicationSettings *find(std::vector<std::string> &nameParts) const
		{
		
		}

		void ServerApplicationsTree::collectApplicationSettings(std::unordered_set<ServerApplicationSettings *> &set) const
		{
		
		}





}
