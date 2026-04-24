#include "IBaseClientDLL.h"

ClientClass *IBaseClientDLL::GetAllClasses() {
  return Memory::InvokeVTableFunction<ClientClass *>(
      this, r2sdk::IBaseClientDLL::Index::GetAllClasses);
}