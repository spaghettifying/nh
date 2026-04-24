#include "NetVarManager.h"

using namespace NetVarManager;
int NetVarManager::NetVarOffset(RecvTable *pRecvTable, const char *NetVarName) {
  for (int i = 0; i < pRecvTable->m_nProps; i++) {
    RecvProp *pRecvProp = pRecvTable->m_pProps[i];

    if (strcmp(pRecvProp->m_pVarName, NetVarName) == 0)
      return pRecvProp->get_offset();

    RecvTable *pNextRecvTable = pRecvProp->GetDataTable();

    if (pRecvProp->GetType() == DPT_DataTable && pNextRecvTable) {
      int currNetVarOffset = ::NetVarOffset(pNextRecvTable, NetVarName);

      if (currNetVarOffset != 0)
        return currNetVarOffset + pRecvProp->get_offset();
    }
  }

  return 0;
}

int NetVarManager::GetNetVar(const char *TableName, const char *NetVarName) {
  ClientClass *pClientClass = g_Interfaces.Client->GetAllClasses();
  for (ClientClass *curr = pClientClass; curr; curr = curr->m_pNext) {
    if (strcmp(TableName, curr->m_pNetworkName) == 0)
      return ::NetVarOffset(curr->m_pRecvTable, NetVarName);
  }

  return 0;
}

void NetVarManager::Dump() {
  if (!g_nssys)
    return;

  // Helper function for recursive dumping
  auto next_dump = [](auto &self, RecvTable *pRecvTable, int level) -> void {
    for (int i = 0; i < pRecvTable->m_nProps; i++) {
      RecvProp *pRecvProp = pRecvTable->m_pProps[i];

      if (isdigit(*pRecvProp->GetName()))
        continue;

      // Build indentation string
      std::string indentation(level * 2, ' '); // 2 spaces per level

      DVariant variant{};
      variant.m_Type = pRecvProp->GetType();

      // Use fmt for formatting instead of stringstream
      std::string msg = fmt::format("{}{:03}. + {:04x} {} {}", indentation, i,
                                    pRecvProp->get_offset(),
                                    pRecvProp->GetName(), variant.ToString());

      g_nssys->Log(g_handle, LogLevel::INFO, msg.c_str());

      RecvTable *pNextRecvTable = pRecvProp->GetDataTable();

      if (pRecvProp->GetType() == DPT_DataTable && pNextRecvTable)
        self(self, pNextRecvTable, level + 1);
    }
  };

  // Actually call the function to start dumping
  ClientClass *pClientClass = g_Interfaces.Client->GetAllClasses();
  for (ClientClass *curr = pClientClass; curr; curr = curr->m_pNext) {
    g_nssys->Log(g_handle, LogLevel::INFO,
                 fmt::format("Class: {}", curr->GetName()).c_str());
    next_dump(next_dump, curr->m_pRecvTable, 0);
  }
}