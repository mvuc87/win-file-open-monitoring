#include <Windows.h>
#include <projectedfslib.h>
#include <tchar.h>
#include <iostream>

#pragma comment(lib, "ProjectedFSLib.lib")

void ErrorDescription(HRESULT hr) {
	if (FACILITY_WINDOWS == HRESULT_FACILITY(hr))
		hr = HRESULT_CODE(hr);
	TCHAR* szErrMsg;

	if (FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)& szErrMsg, 0, NULL) != 0) {
		_tprintf(TEXT("%s"), szErrMsg);
		LocalFree(szErrMsg);
	} else
		_tprintf(TEXT("[Could not find a description for error # %#x.]\n"), hr);
}

PRJ_START_DIRECTORY_ENUMERATION_CB MyStartEnumCallback;

HRESULT CALLBACK MyStartEnumCallback(
	const PRJ_CALLBACK_DATA* callbackData,
	const GUID* enumerationId
) {
	std::cout << "Starting enumeration" << std::endl;

	return S_OK;
}

PRJ_GET_DIRECTORY_ENUMERATION_CB MyGetEnumCallback;

HRESULT CALLBACK MyGetEnumCallback(
	const PRJ_CALLBACK_DATA* callbackData,
	const GUID* enumerationId,
	PCWSTR searchExpression,
	PRJ_DIR_ENTRY_BUFFER_HANDLE dirEntryBufferHandle
) {
	return S_OK;
}

PRJ_END_DIRECTORY_ENUMERATION_CB MyEndEnumCallback;

HRESULT CALLBACK MyEndEnumCallback(
	const PRJ_CALLBACK_DATA* callbackData,
	const GUID* enumerationId
) {
	return S_OK;
}

PRJ_NOTIFICATION_CB MyNotificationCallback;

HRESULT CALLBACK MyNotificationCallback(
	const PRJ_CALLBACK_DATA* callbackData,
	BOOLEAN isDirectory,
	PRJ_NOTIFICATION notification,
	PCWSTR destinationFileName,
	PRJ_NOTIFICATION_PARAMETERS* operationParameters
) {
	std::cout << "file: " << callbackData->FilePathName << std::endl;
	std::cout << "isdir: " << isDirectory << std::endl;

	if (notification == PRJ_NOTIFICATION_FILE_OPENED) {
		std::cout << "event: file opened" << std::endl;
	}

	return S_OK;
}

int main() {

	GUID instanceId;
	HRESULT guidResult = CoCreateGuid(&instanceId);
	if (FAILED(guidResult)) {
		std::cerr << "Failed to create a GUID";
    ErrorDescription(guidResult);
		return 0;
	}

	HRESULT dirAsPlaceholder = PrjMarkDirectoryAsPlaceholder(L"C:\\temp\\projected", nullptr, nullptr, &instanceId);
	if (FAILED(dirAsPlaceholder)) {
		std::cerr << "Failed to assign virtualziation root directory to the virtualization instance." << std::endl;
		ErrorDescription(dirAsPlaceholder);
		return 0;
	}

	PRJ_CALLBACKS callbackTable;
	callbackTable.StartDirectoryEnumerationCallback = MyStartEnumCallback;
	callbackTable.EndDirectoryEnumerationCallback = MyEndEnumCallback;
	callbackTable.GetDirectoryEnumerationCallback = MyGetEnumCallback;
	callbackTable.GetPlaceholderInfoCallback = nullptr;
	callbackTable.GetFileDataCallback = nullptr;

	callbackTable.QueryFileNameCallback = nullptr;
	callbackTable.NotificationCallback = MyNotificationCallback;
	callbackTable.CancelCommandCallback = nullptr;

	PRJ_NOTIFICATION_MAPPING notificationMappings[1];
	notificationMappings[0].NotificationRoot = L"";
	notificationMappings[0].NotificationBitMask = PRJ_NOTIFY_FILE_OPENED;

	PRJ_STARTVIRTUALIZING_OPTIONS startOpts = {};
	startOpts.NotificationMappings = notificationMappings;
	startOpts.NotificationMappingsCount = ARRAYSIZE(notificationMappings);

	PRJ_NAMESPACE_VIRTUALIZATION_CONTEXT instanceHandle;
	HRESULT startVirt = PrjStartVirtualizing(L"C:\\temp\\projected", &callbackTable, nullptr, &startOpts, &instanceHandle);
	if (FAILED(startVirt)) {
		ErrorDescription(startVirt);
		return 0;
	}

	while (true) {} // :)
}
