/*
    Copyright 2001 Justin Kirby
    This file is part of Outport.

    Outport is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    Outport is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with olexp; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/
// Machine generated IDispatch wrapper class(es) created with ClassWizard
/////////////////////////////////////////////////////////////////////////////
// _Application wrapper class
#ifndef OLEXP_MSOUTL_H
#define OLEXP_MSOUTL_H

class _Application : public COleDispatchDriver
{
public:
	_Application() {}		// Calls COleDispatchDriver default constructor
	_Application(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	_Application(const _Application& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	LPDISPATCH GetApplication();
	long GetClass();
	LPDISPATCH GetSession();
	LPDISPATCH GetParent();
	LPDISPATCH GetAssistant();
	CString GetName();
	CString GetVersion();
	LPDISPATCH ActiveExplorer();
	LPDISPATCH ActiveInspector();
	LPDISPATCH CreateItem(long ItemType);
	LPDISPATCH CreateItemFromTemplate(LPCTSTR TemplatePath, const VARIANT& InFolder);
	LPDISPATCH CreateObject(LPCTSTR ObjectName);
	LPDISPATCH GetNamespace(LPCTSTR Type);
	void Quit();
	LPDISPATCH GetCOMAddIns();
	LPDISPATCH GetExplorers();
	LPDISPATCH GetInspectors();
	LPDISPATCH GetLanguageSettings();
	CString GetProductCode();
	LPDISPATCH GetAnswerWizard();
	LPDISPATCH ActiveWindow();
};
/////////////////////////////////////////////////////////////////////////////
// _Folders wrapper class

class _Folders : public COleDispatchDriver
{
public:
	_Folders() {}		// Calls COleDispatchDriver default constructor
	_Folders(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	_Folders(const _Folders& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	LPDISPATCH GetApplication();
	long GetClass();
	LPDISPATCH GetSession();
	LPDISPATCH GetParent();
	long GetCount();
	LPDISPATCH Item(const VARIANT& Index);
	LPDISPATCH Add(LPCTSTR Name, const VARIANT& Type);
	LPDISPATCH GetFirst();
	LPDISPATCH GetLast();
	LPDISPATCH GetNext();
	LPDISPATCH GetPrevious();
	void Remove(long Index);
};
/////////////////////////////////////////////////////////////////////////////
// MAPIFolder wrapper class

class MAPIFolder : public COleDispatchDriver
{
public:
	MAPIFolder() {}		// Calls COleDispatchDriver default constructor
	MAPIFolder(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	MAPIFolder(const MAPIFolder& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	LPDISPATCH GetApplication();
	long GetClass();
	LPDISPATCH GetSession();
	LPDISPATCH GetParent();
	long GetDefaultItemType();
	CString GetDefaultMessageClass();
	CString GetDescription();
	void SetDescription(LPCTSTR lpszNewValue);
	CString GetEntryID();
	LPDISPATCH GetFolders();
	LPDISPATCH GetItems();
	CString GetName();
	void SetName(LPCTSTR lpszNewValue);
	CString GetStoreID();
	long GetUnReadItemCount();
	LPDISPATCH CopyTo(LPDISPATCH DestinationFolder);
	void Delete();
	void Display();
	LPDISPATCH GetExplorer(const VARIANT& DisplayMode);
	void MoveTo(LPDISPATCH DestinationFolder);
	BOOL GetWebViewOn();
	void SetWebViewOn(BOOL bNewValue);
	CString GetWebViewURL();
	void SetWebViewURL(LPCTSTR lpszNewValue);
	BOOL GetWebViewAllowNavigation();
	void SetWebViewAllowNavigation(BOOL bNewValue);
};
/////////////////////////////////////////////////////////////////////////////
// _NameSpace wrapper class

class _NameSpace : public COleDispatchDriver
{
public:
	_NameSpace() {}		// Calls COleDispatchDriver default constructor
	_NameSpace(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	_NameSpace(const _NameSpace& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	LPDISPATCH GetApplication();
	long GetClass();
	LPDISPATCH GetSession();
	LPDISPATCH GetParent();
	LPDISPATCH GetCurrentUser();
	LPDISPATCH GetFolders();
	CString GetType();
	LPDISPATCH GetAddressLists();
	LPDISPATCH CreateRecipient(LPCTSTR RecipientName);
	LPDISPATCH GetDefaultFolder(long FolderType);
	LPDISPATCH GetFolderFromID(LPCTSTR EntryIDFolder, const VARIANT& EntryIDStore);
	LPDISPATCH GetItemFromID(LPCTSTR EntryIDItem, const VARIANT& EntryIDStore);
	LPDISPATCH GetRecipientFromID(LPCTSTR EntryID);
	LPDISPATCH GetSharedDefaultFolder(LPDISPATCH Recipient, long FolderType);
	void Logoff();
	void Logon(const VARIANT& Profile, const VARIANT& Password, const VARIANT& ShowDialog, const VARIANT& NewSession);
	LPDISPATCH PickFolder();
	LPDISPATCH GetSyncObjects();
	void AddStore(const VARIANT& Store);
};
/////////////////////////////////////////////////////////////////////////////
// _Items wrapper class

class _Items : public COleDispatchDriver
{
public:
	_Items() {}		// Calls COleDispatchDriver default constructor
	_Items(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	_Items(const _Items& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	LPDISPATCH GetApplication();
	long GetClass();
	LPDISPATCH GetSession();
	LPDISPATCH GetParent();
	long GetCount();
	LPDISPATCH Item(const VARIANT& Index);
	BOOL GetIncludeRecurrences();
	void SetIncludeRecurrences(BOOL bNewValue);
	LPDISPATCH Add(const VARIANT& Type);
	LPDISPATCH Find(LPCTSTR Filter);
	LPDISPATCH FindNext();
	LPDISPATCH GetFirst();
	LPDISPATCH GetLast();
	LPDISPATCH GetNext();
	LPDISPATCH GetPrevious();
	void Remove(long Index);
	void ResetColumns();
	LPDISPATCH Restrict(LPCTSTR Filter);
	void SetColumns(LPCTSTR Columns);
	void Sort(LPCTSTR Property_, const VARIANT& Descending);
};
/////////////////////////////////////////////////////////////////////////////
// _TaskItem wrapper class

class _TaskItem : public COleDispatchDriver
{
public:
	_TaskItem() {}		// Calls COleDispatchDriver default constructor
	_TaskItem(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	_TaskItem(const _TaskItem& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	LPDISPATCH GetApplication();
	long GetClass();
	LPDISPATCH GetSession();
	LPDISPATCH GetParent();
	LPDISPATCH GetActions();
	LPDISPATCH GetAttachments();
	CString GetBillingInformation();
	void SetBillingInformation(LPCTSTR lpszNewValue);
	CString GetBody();
	void SetBody(LPCTSTR lpszNewValue);
	CString GetCategories();
	void SetCategories(LPCTSTR lpszNewValue);
	CString GetCompanies();
	void SetCompanies(LPCTSTR lpszNewValue);
	CString GetConversationIndex();
	CString GetConversationTopic();
	DATE GetCreationTime();
	CString GetEntryID();
	LPDISPATCH GetFormDescription();
	LPDISPATCH GetGetInspector();
	long GetImportance();
	void SetImportance(long nNewValue);
	DATE GetLastModificationTime();
	CString GetMessageClass();
	void SetMessageClass(LPCTSTR lpszNewValue);
	CString GetMileage();
	void SetMileage(LPCTSTR lpszNewValue);
	BOOL GetNoAging();
	void SetNoAging(BOOL bNewValue);
	long GetOutlookInternalVersion();
	CString GetOutlookVersion();
	BOOL GetSaved();
	long GetSensitivity();
	void SetSensitivity(long nNewValue);
	long GetSize();
	CString GetSubject();
	void SetSubject(LPCTSTR lpszNewValue);
	BOOL GetUnRead();
	void SetUnRead(BOOL bNewValue);
	LPDISPATCH GetUserProperties();
	void Close(long SaveMode);
	LPDISPATCH Copy();
	void Delete();
	void Display(const VARIANT& Modal);
	LPDISPATCH Move(LPDISPATCH DestFldr);
	void PrintOut();
	void Save();
	void SaveAs(LPCTSTR Path, const VARIANT& Type);
	long GetActualWork();
	void SetActualWork(long nNewValue);
	CString GetCardData();
	void SetCardData(LPCTSTR lpszNewValue);
	BOOL GetComplete();
	void SetComplete(BOOL bNewValue);
	CString GetContacts();
	void SetContacts(LPCTSTR lpszNewValue);
	CString GetContactNames();
	void SetContactNames(LPCTSTR lpszNewValue);
	DATE GetDateCompleted();
	void SetDateCompleted(DATE newValue);
	long GetDelegationState();
	CString GetDelegator();
	DATE GetDueDate();
	void SetDueDate(DATE newValue);
	BOOL GetIsRecurring();
	long GetOrdinal();
	void SetOrdinal(long nNewValue);
	CString GetOwner();
	void SetOwner(LPCTSTR lpszNewValue);
	long GetOwnership();
	long GetPercentComplete();
	void SetPercentComplete(long nNewValue);
	LPDISPATCH GetRecipients();
	DATE GetReminderTime();
	void SetReminderTime(DATE newValue);
	BOOL GetReminderOverrideDefault();
	void SetReminderOverrideDefault(BOOL bNewValue);
	BOOL GetReminderPlaySound();
	void SetReminderPlaySound(BOOL bNewValue);
	BOOL GetReminderSet();
	void SetReminderSet(BOOL bNewValue);
	CString GetReminderSoundFile();
	void SetReminderSoundFile(LPCTSTR lpszNewValue);
	long GetResponseState();
	CString GetRole();
	void SetRole(LPCTSTR lpszNewValue);
	CString GetSchedulePlusPriority();
	void SetSchedulePlusPriority(LPCTSTR lpszNewValue);
	DATE GetStartDate();
	void SetStartDate(DATE newValue);
	long GetStatus();
	void SetStatus(long nNewValue);
	CString GetStatusOnCompletionRecipients();
	void SetStatusOnCompletionRecipients(LPCTSTR lpszNewValue);
	CString GetStatusUpdateRecipients();
	void SetStatusUpdateRecipients(LPCTSTR lpszNewValue);
	BOOL GetTeamTask();
	void SetTeamTask(BOOL bNewValue);
	long GetTotalWork();
	void SetTotalWork(long nNewValue);
	LPDISPATCH Assign();
	void CancelResponseState();
	void ClearRecurrencePattern();
	LPDISPATCH GetRecurrencePattern();
	void MarkComplete();
	LPDISPATCH Respond(long Response, const VARIANT& fNoUI, const VARIANT& fAdditionalTextDialog);
	void Send();
	BOOL SkipRecurrence();
	LPDISPATCH StatusReport();
	LPDISPATCH GetLinks();
};
/////////////////////////////////////////////////////////////////////////////
// _AppointmentItem wrapper class

class _AppointmentItem : public COleDispatchDriver
{
public:
	_AppointmentItem() {}		// Calls COleDispatchDriver default constructor
	_AppointmentItem(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	_AppointmentItem(const _AppointmentItem& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	LPDISPATCH GetApplication();
	long GetClass();
	LPDISPATCH GetSession();
	LPDISPATCH GetParent();
	LPDISPATCH GetActions();
	LPDISPATCH GetAttachments();
	CString GetBillingInformation();
	void SetBillingInformation(LPCTSTR lpszNewValue);
	CString GetBody();
	void SetBody(LPCTSTR lpszNewValue);
	CString GetCategories();
	void SetCategories(LPCTSTR lpszNewValue);
	CString GetCompanies();
	void SetCompanies(LPCTSTR lpszNewValue);
	CString GetConversationIndex();
	CString GetConversationTopic();
	DATE GetCreationTime();
	CString GetEntryID();
	LPDISPATCH GetFormDescription();
	LPDISPATCH GetGetInspector();
	long GetImportance();
	void SetImportance(long nNewValue);
	DATE GetLastModificationTime();
	CString GetMessageClass();
	void SetMessageClass(LPCTSTR lpszNewValue);
	CString GetMileage();
	void SetMileage(LPCTSTR lpszNewValue);
	BOOL GetNoAging();
	void SetNoAging(BOOL bNewValue);
	long GetOutlookInternalVersion();
	CString GetOutlookVersion();
	BOOL GetSaved();
	long GetSensitivity();
	void SetSensitivity(long nNewValue);
	long GetSize();
	CString GetSubject();
	void SetSubject(LPCTSTR lpszNewValue);
	BOOL GetUnRead();
	void SetUnRead(BOOL bNewValue);
	LPDISPATCH GetUserProperties();
	void Close(long SaveMode);
	LPDISPATCH Copy();
	void Delete();
	void Display(const VARIANT& Modal);
	LPDISPATCH Move(LPDISPATCH DestFldr);
	void PrintOut();
	void Save();
	void SaveAs(LPCTSTR Path, const VARIANT& Type);
	BOOL GetAllDayEvent();
	void SetAllDayEvent(BOOL bNewValue);
	long GetBusyStatus();
	void SetBusyStatus(long nNewValue);
	long GetDuration();
	void SetDuration(long nNewValue);
	DATE GetEnd();
	void SetEnd(DATE newValue);
	BOOL GetIsOnlineMeeting();
	void SetIsOnlineMeeting(BOOL bNewValue);
	BOOL GetIsRecurring();
	CString GetLocation();
	void SetLocation(LPCTSTR lpszNewValue);
	long GetMeetingStatus();
	void SetMeetingStatus(long nNewValue);
	BOOL GetNetMeetingAutoStart();
	void SetNetMeetingAutoStart(BOOL bNewValue);
	CString GetNetMeetingOrganizerAlias();
	void SetNetMeetingOrganizerAlias(LPCTSTR lpszNewValue);
	CString GetNetMeetingServer();
	void SetNetMeetingServer(LPCTSTR lpszNewValue);
	long GetNetMeetingType();
	void SetNetMeetingType(long nNewValue);
	CString GetOptionalAttendees();
	void SetOptionalAttendees(LPCTSTR lpszNewValue);
	CString GetOrganizer();
	LPDISPATCH GetRecipients();
	long GetRecurrenceState();
	long GetReminderMinutesBeforeStart();
	void SetReminderMinutesBeforeStart(long nNewValue);
	BOOL GetReminderOverrideDefault();
	void SetReminderOverrideDefault(BOOL bNewValue);
	BOOL GetReminderPlaySound();
	void SetReminderPlaySound(BOOL bNewValue);
	BOOL GetReminderSet();
	void SetReminderSet(BOOL bNewValue);
	CString GetReminderSoundFile();
	void SetReminderSoundFile(LPCTSTR lpszNewValue);
	DATE GetReplyTime();
	void SetReplyTime(DATE newValue);
	CString GetRequiredAttendees();
	void SetRequiredAttendees(LPCTSTR lpszNewValue);
	CString GetResources();
	void SetResources(LPCTSTR lpszNewValue);
	BOOL GetResponseRequested();
	void SetResponseRequested(BOOL bNewValue);
	long GetResponseStatus();
	DATE GetStart();
	void SetStart(DATE newValue);
	void ClearRecurrencePattern();
	LPDISPATCH ForwardAsVcal();
	LPDISPATCH GetRecurrencePattern();
	LPDISPATCH Respond(long Response, const VARIANT& fNoUI, const VARIANT& fAdditionalTextDialog);
	void Send();
	CString GetNetMeetingDocPathName();
	void SetNetMeetingDocPathName(LPCTSTR lpszNewValue);
	CString GetNetShowURL();
	void SetNetShowURL(LPCTSTR lpszNewValue);
	LPDISPATCH GetLinks();
	BOOL GetConferenceServerAllowExternal();
	void SetConferenceServerAllowExternal(BOOL bNewValue);
	CString GetConferenceServerPassword();
	void SetConferenceServerPassword(LPCTSTR lpszNewValue);
};
/////////////////////////////////////////////////////////////////////////////
// _MailItem wrapper class

class _MailItem : public COleDispatchDriver
{
public:
	_MailItem() {}		// Calls COleDispatchDriver default constructor
	_MailItem(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	_MailItem(const _MailItem& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	LPDISPATCH GetApplication();
	long GetClass();
	LPDISPATCH GetSession();
	LPDISPATCH GetParent();
	LPDISPATCH GetActions();
	LPDISPATCH GetAttachments();
	CString GetBillingInformation();
	void SetBillingInformation(LPCTSTR lpszNewValue);
	CString GetBody();
	void SetBody(LPCTSTR lpszNewValue);
	CString GetCategories();
	void SetCategories(LPCTSTR lpszNewValue);
	CString GetCompanies();
	void SetCompanies(LPCTSTR lpszNewValue);
	CString GetConversationIndex();
	CString GetConversationTopic();
	DATE GetCreationTime();
	CString GetEntryID();
	LPDISPATCH GetFormDescription();
	LPDISPATCH GetGetInspector();
	long GetImportance();
	void SetImportance(long nNewValue);
	DATE GetLastModificationTime();
	CString GetMessageClass();
	void SetMessageClass(LPCTSTR lpszNewValue);
	CString GetMileage();
	void SetMileage(LPCTSTR lpszNewValue);
	BOOL GetNoAging();
	void SetNoAging(BOOL bNewValue);
	long GetOutlookInternalVersion();
	CString GetOutlookVersion();
	BOOL GetSaved();
	long GetSensitivity();
	void SetSensitivity(long nNewValue);
	long GetSize();
	CString GetSubject();
	void SetSubject(LPCTSTR lpszNewValue);
	BOOL GetUnRead();
	void SetUnRead(BOOL bNewValue);
	LPDISPATCH GetUserProperties();
	void Close(long SaveMode);
	LPDISPATCH Copy();
	void Delete();
	void Display(const VARIANT& Modal);
	LPDISPATCH Move(LPDISPATCH DestFldr);
	void PrintOut();
	void Save();
	void SaveAs(LPCTSTR Path, const VARIANT& Type);
	BOOL GetAlternateRecipientAllowed();
	void SetAlternateRecipientAllowed(BOOL bNewValue);
	BOOL GetAutoForwarded();
	void SetAutoForwarded(BOOL bNewValue);
	CString GetBcc();
	void SetBcc(LPCTSTR lpszNewValue);
	CString GetCc();
	void SetCc(LPCTSTR lpszNewValue);
	DATE GetDeferredDeliveryTime();
	void SetDeferredDeliveryTime(DATE newValue);
	BOOL GetDeleteAfterSubmit();
	void SetDeleteAfterSubmit(BOOL bNewValue);
	DATE GetExpiryTime();
	void SetExpiryTime(DATE newValue);
	DATE GetFlagDueBy();
	void SetFlagDueBy(DATE newValue);
	CString GetFlagRequest();
	void SetFlagRequest(LPCTSTR lpszNewValue);
	long GetFlagStatus();
	void SetFlagStatus(long nNewValue);
	CString GetHTMLBody();
	void SetHTMLBody(LPCTSTR lpszNewValue);
	BOOL GetOriginatorDeliveryReportRequested();
	void SetOriginatorDeliveryReportRequested(BOOL bNewValue);
	BOOL GetReadReceiptRequested();
	void SetReadReceiptRequested(BOOL bNewValue);
	CString GetReceivedByEntryID();
	CString GetReceivedByName();
	CString GetReceivedOnBehalfOfEntryID();
	CString GetReceivedOnBehalfOfName();
	DATE GetReceivedTime();
	BOOL GetRecipientReassignmentProhibited();
	void SetRecipientReassignmentProhibited(BOOL bNewValue);
	LPDISPATCH GetRecipients();
	BOOL GetReminderOverrideDefault();
	void SetReminderOverrideDefault(BOOL bNewValue);
	BOOL GetReminderPlaySound();
	void SetReminderPlaySound(BOOL bNewValue);
	BOOL GetReminderSet();
	void SetReminderSet(BOOL bNewValue);
	CString GetReminderSoundFile();
	void SetReminderSoundFile(LPCTSTR lpszNewValue);
	DATE GetReminderTime();
	void SetReminderTime(DATE newValue);
	long GetRemoteStatus();
	void SetRemoteStatus(long nNewValue);
	CString GetReplyRecipientNames();
	LPDISPATCH GetReplyRecipients();
	LPDISPATCH GetSaveSentMessageFolder();
	void SetRefSaveSentMessageFolder(LPDISPATCH newValue);
	CString GetSenderName();
	BOOL GetSent();
	DATE GetSentOn();
	CString GetSentOnBehalfOfName();
	void SetSentOnBehalfOfName(LPCTSTR lpszNewValue);
	BOOL GetSubmitted();
	CString GetTo();
	void SetTo(LPCTSTR lpszNewValue);
	CString GetVotingOptions();
	void SetVotingOptions(LPCTSTR lpszNewValue);
	CString GetVotingResponse();
	void SetVotingResponse(LPCTSTR lpszNewValue);
	void ClearConversationIndex();
	LPDISPATCH Forward();
	LPDISPATCH Reply();
	LPDISPATCH ReplyAll();
	void Send();
	LPDISPATCH GetLinks();
};
/////////////////////////////////////////////////////////////////////////////
// _ContactItem wrapper class

class _ContactItem : public COleDispatchDriver
{
public:
	_ContactItem() {}		// Calls COleDispatchDriver default constructor
	_ContactItem(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	_ContactItem(const _ContactItem& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	LPDISPATCH GetApplication();
	long GetClass();
	LPDISPATCH GetSession();
	LPDISPATCH GetParent();
	LPDISPATCH GetActions();
	LPDISPATCH GetAttachments();
	CString GetBillingInformation();
	void SetBillingInformation(LPCTSTR lpszNewValue);
	CString GetBody();
	void SetBody(LPCTSTR lpszNewValue);
	CString GetCategories();
	void SetCategories(LPCTSTR lpszNewValue);
	CString GetCompanies();
	void SetCompanies(LPCTSTR lpszNewValue);
	CString GetConversationIndex();
	CString GetConversationTopic();
	DATE GetCreationTime();
	CString GetEntryID();
	LPDISPATCH GetFormDescription();
	LPDISPATCH GetGetInspector();
	long GetImportance();
	void SetImportance(long nNewValue);
	DATE GetLastModificationTime();
	CString GetMessageClass();
	void SetMessageClass(LPCTSTR lpszNewValue);
	CString GetMileage();
	void SetMileage(LPCTSTR lpszNewValue);
	BOOL GetNoAging();
	void SetNoAging(BOOL bNewValue);
	long GetOutlookInternalVersion();
	CString GetOutlookVersion();
	BOOL GetSaved();
	long GetSensitivity();
	void SetSensitivity(long nNewValue);
	long GetSize();
	CString GetSubject();
	void SetSubject(LPCTSTR lpszNewValue);
	BOOL GetUnRead();
	void SetUnRead(BOOL bNewValue);
	LPDISPATCH GetUserProperties();
	void Close(long SaveMode);
	LPDISPATCH Copy();
	void Delete();
	void Display(const VARIANT& Modal);
	LPDISPATCH Move(LPDISPATCH DestFldr);
	void PrintOut();
	void Save();
	void SaveAs(LPCTSTR Path, const VARIANT& Type);
	CString GetAccount();
	void SetAccount(LPCTSTR lpszNewValue);
	DATE GetAnniversary();
	void SetAnniversary(DATE newValue);
	CString GetAssistantName();
	void SetAssistantName(LPCTSTR lpszNewValue);
	CString GetAssistantTelephoneNumber();
	void SetAssistantTelephoneNumber(LPCTSTR lpszNewValue);
	DATE GetBirthday();
	void SetBirthday(DATE newValue);
	CString GetBusiness2TelephoneNumber();
	void SetBusiness2TelephoneNumber(LPCTSTR lpszNewValue);
	CString GetBusinessAddress();
	void SetBusinessAddress(LPCTSTR lpszNewValue);
	CString GetBusinessAddressCity();
	void SetBusinessAddressCity(LPCTSTR lpszNewValue);
	CString GetBusinessAddressCountry();
	void SetBusinessAddressCountry(LPCTSTR lpszNewValue);
	CString GetBusinessAddressPostalCode();
	void SetBusinessAddressPostalCode(LPCTSTR lpszNewValue);
	CString GetBusinessAddressPostOfficeBox();
	void SetBusinessAddressPostOfficeBox(LPCTSTR lpszNewValue);
	CString GetBusinessAddressState();
	void SetBusinessAddressState(LPCTSTR lpszNewValue);
	CString GetBusinessAddressStreet();
	void SetBusinessAddressStreet(LPCTSTR lpszNewValue);
	CString GetBusinessFaxNumber();
	void SetBusinessFaxNumber(LPCTSTR lpszNewValue);
	CString GetBusinessHomePage();
	void SetBusinessHomePage(LPCTSTR lpszNewValue);
	CString GetBusinessTelephoneNumber();
	void SetBusinessTelephoneNumber(LPCTSTR lpszNewValue);
	CString GetCallbackTelephoneNumber();
	void SetCallbackTelephoneNumber(LPCTSTR lpszNewValue);
	CString GetCarTelephoneNumber();
	void SetCarTelephoneNumber(LPCTSTR lpszNewValue);
	CString GetChildren();
	void SetChildren(LPCTSTR lpszNewValue);
	CString GetCompanyAndFullName();
	CString GetCompanyLastFirstNoSpace();
	CString GetCompanyLastFirstSpaceOnly();
	CString GetCompanyMainTelephoneNumber();
	void SetCompanyMainTelephoneNumber(LPCTSTR lpszNewValue);
	CString GetCompanyName();
	void SetCompanyName(LPCTSTR lpszNewValue);
	CString GetComputerNetworkName();
	void SetComputerNetworkName(LPCTSTR lpszNewValue);
	CString GetCustomerID();
	void SetCustomerID(LPCTSTR lpszNewValue);
	CString GetDepartment();
	void SetDepartment(LPCTSTR lpszNewValue);
	CString GetEmail1Address();
	void SetEmail1Address(LPCTSTR lpszNewValue);
	CString GetEmail1AddressType();
	void SetEmail1AddressType(LPCTSTR lpszNewValue);
	CString GetEmail1DisplayName();
	CString GetEmail1EntryID();
	CString GetEmail2Address();
	void SetEmail2Address(LPCTSTR lpszNewValue);
	CString GetEmail2AddressType();
	void SetEmail2AddressType(LPCTSTR lpszNewValue);
	CString GetEmail2DisplayName();
	CString GetEmail2EntryID();
	CString GetEmail3Address();
	void SetEmail3Address(LPCTSTR lpszNewValue);
	CString GetEmail3AddressType();
	void SetEmail3AddressType(LPCTSTR lpszNewValue);
	CString GetEmail3DisplayName();
	CString GetEmail3EntryID();
	CString GetFileAs();
	void SetFileAs(LPCTSTR lpszNewValue);
	CString GetFirstName();
	void SetFirstName(LPCTSTR lpszNewValue);
	CString GetFTPSite();
	void SetFTPSite(LPCTSTR lpszNewValue);
	CString GetFullName();
	void SetFullName(LPCTSTR lpszNewValue);
	CString GetFullNameAndCompany();
	long GetGender();
	void SetGender(long nNewValue);
	CString GetGovernmentIDNumber();
	void SetGovernmentIDNumber(LPCTSTR lpszNewValue);
	CString GetHobby();
	void SetHobby(LPCTSTR lpszNewValue);
	CString GetHome2TelephoneNumber();
	void SetHome2TelephoneNumber(LPCTSTR lpszNewValue);
	CString GetHomeAddress();
	void SetHomeAddress(LPCTSTR lpszNewValue);
	CString GetHomeAddressCity();
	void SetHomeAddressCity(LPCTSTR lpszNewValue);
	CString GetHomeAddressCountry();
	void SetHomeAddressCountry(LPCTSTR lpszNewValue);
	CString GetHomeAddressPostalCode();
	void SetHomeAddressPostalCode(LPCTSTR lpszNewValue);
	CString GetHomeAddressPostOfficeBox();
	void SetHomeAddressPostOfficeBox(LPCTSTR lpszNewValue);
	CString GetHomeAddressState();
	void SetHomeAddressState(LPCTSTR lpszNewValue);
	CString GetHomeAddressStreet();
	void SetHomeAddressStreet(LPCTSTR lpszNewValue);
	CString GetHomeFaxNumber();
	void SetHomeFaxNumber(LPCTSTR lpszNewValue);
	CString GetHomeTelephoneNumber();
	void SetHomeTelephoneNumber(LPCTSTR lpszNewValue);
	CString GetInitials();
	void SetInitials(LPCTSTR lpszNewValue);
	CString GetInternetFreeBusyAddress();
	void SetInternetFreeBusyAddress(LPCTSTR lpszNewValue);
	CString GetISDNNumber();
	void SetISDNNumber(LPCTSTR lpszNewValue);
	CString GetJobTitle();
	void SetJobTitle(LPCTSTR lpszNewValue);
	BOOL GetJournal();
	void SetJournal(BOOL bNewValue);
	CString GetLanguage();
	void SetLanguage(LPCTSTR lpszNewValue);
	CString GetLastFirstAndSuffix();
	CString GetLastFirstNoSpace();
	CString GetLastFirstNoSpaceCompany();
	CString GetLastFirstSpaceOnly();
	CString GetLastFirstSpaceOnlyCompany();
	CString GetLastName();
	void SetLastName(LPCTSTR lpszNewValue);
	CString GetLastNameAndFirstName();
	CString GetMailingAddress();
	void SetMailingAddress(LPCTSTR lpszNewValue);
	CString GetMailingAddressCity();
	void SetMailingAddressCity(LPCTSTR lpszNewValue);
	CString GetMailingAddressCountry();
	void SetMailingAddressCountry(LPCTSTR lpszNewValue);
	CString GetMailingAddressPostalCode();
	void SetMailingAddressPostalCode(LPCTSTR lpszNewValue);
	CString GetMailingAddressPostOfficeBox();
	void SetMailingAddressPostOfficeBox(LPCTSTR lpszNewValue);
	CString GetMailingAddressState();
	void SetMailingAddressState(LPCTSTR lpszNewValue);
	CString GetMailingAddressStreet();
	void SetMailingAddressStreet(LPCTSTR lpszNewValue);
	CString GetManagerName();
	void SetManagerName(LPCTSTR lpszNewValue);
	CString GetMiddleName();
	void SetMiddleName(LPCTSTR lpszNewValue);
	CString GetMobileTelephoneNumber();
	void SetMobileTelephoneNumber(LPCTSTR lpszNewValue);
	CString GetNetMeetingAlias();
	void SetNetMeetingAlias(LPCTSTR lpszNewValue);
	CString GetNetMeetingServer();
	void SetNetMeetingServer(LPCTSTR lpszNewValue);
	CString GetNickName();
	void SetNickName(LPCTSTR lpszNewValue);
	CString GetOfficeLocation();
	void SetOfficeLocation(LPCTSTR lpszNewValue);
	CString GetOrganizationalIDNumber();
	void SetOrganizationalIDNumber(LPCTSTR lpszNewValue);
	CString GetOtherAddress();
	void SetOtherAddress(LPCTSTR lpszNewValue);
	CString GetOtherAddressCity();
	void SetOtherAddressCity(LPCTSTR lpszNewValue);
	CString GetOtherAddressCountry();
	void SetOtherAddressCountry(LPCTSTR lpszNewValue);
	CString GetOtherAddressPostalCode();
	void SetOtherAddressPostalCode(LPCTSTR lpszNewValue);
	CString GetOtherAddressPostOfficeBox();
	void SetOtherAddressPostOfficeBox(LPCTSTR lpszNewValue);
	CString GetOtherAddressState();
	void SetOtherAddressState(LPCTSTR lpszNewValue);
	CString GetOtherAddressStreet();
	void SetOtherAddressStreet(LPCTSTR lpszNewValue);
	CString GetOtherFaxNumber();
	void SetOtherFaxNumber(LPCTSTR lpszNewValue);
	CString GetOtherTelephoneNumber();
	void SetOtherTelephoneNumber(LPCTSTR lpszNewValue);
	CString GetPagerNumber();
	void SetPagerNumber(LPCTSTR lpszNewValue);
	CString GetPersonalHomePage();
	void SetPersonalHomePage(LPCTSTR lpszNewValue);
	CString GetPrimaryTelephoneNumber();
	void SetPrimaryTelephoneNumber(LPCTSTR lpszNewValue);
	CString GetProfession();
	void SetProfession(LPCTSTR lpszNewValue);
	CString GetRadioTelephoneNumber();
	void SetRadioTelephoneNumber(LPCTSTR lpszNewValue);
	CString GetReferredBy();
	void SetReferredBy(LPCTSTR lpszNewValue);
	long GetSelectedMailingAddress();
	void SetSelectedMailingAddress(long nNewValue);
	CString GetSpouse();
	void SetSpouse(LPCTSTR lpszNewValue);
	CString GetSuffix();
	void SetSuffix(LPCTSTR lpszNewValue);
	CString GetTelexNumber();
	void SetTelexNumber(LPCTSTR lpszNewValue);
	CString GetTitle();
	void SetTitle(LPCTSTR lpszNewValue);
	CString GetTTYTDDTelephoneNumber();
	void SetTTYTDDTelephoneNumber(LPCTSTR lpszNewValue);
	CString GetUser1();
	void SetUser1(LPCTSTR lpszNewValue);
	CString GetUser2();
	void SetUser2(LPCTSTR lpszNewValue);
	CString GetUser3();
	void SetUser3(LPCTSTR lpszNewValue);
	CString GetUser4();
	void SetUser4(LPCTSTR lpszNewValue);
	CString GetUserCertificate();
	void SetUserCertificate(LPCTSTR lpszNewValue);
	CString GetWebPage();
	void SetWebPage(LPCTSTR lpszNewValue);
	CString GetYomiCompanyName();
	void SetYomiCompanyName(LPCTSTR lpszNewValue);
	CString GetYomiFirstName();
	void SetYomiFirstName(LPCTSTR lpszNewValue);
	CString GetYomiLastName();
	void SetYomiLastName(LPCTSTR lpszNewValue);
	LPDISPATCH ForwardAsVcard();
	LPDISPATCH GetLinks();
};
/////////////////////////////////////////////////////////////////////////////
// _DistListItem wrapper class

class _DistListItem : public COleDispatchDriver
{
public:
	_DistListItem() {}		// Calls COleDispatchDriver default constructor
	_DistListItem(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	_DistListItem(const _DistListItem& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	LPDISPATCH GetApplication();
	long GetClass();
	LPDISPATCH GetSession();
	LPDISPATCH GetParent();
	LPDISPATCH GetActions();
	LPDISPATCH GetAttachments();
	CString GetBillingInformation();
	void SetBillingInformation(LPCTSTR lpszNewValue);
	CString GetBody();
	void SetBody(LPCTSTR lpszNewValue);
	CString GetCategories();
	void SetCategories(LPCTSTR lpszNewValue);
	CString GetCompanies();
	void SetCompanies(LPCTSTR lpszNewValue);
	CString GetConversationIndex();
	CString GetConversationTopic();
	DATE GetCreationTime();
	CString GetEntryID();
	LPDISPATCH GetFormDescription();
	LPDISPATCH GetGetInspector();
	long GetImportance();
	void SetImportance(long nNewValue);
	DATE GetLastModificationTime();
	CString GetMessageClass();
	void SetMessageClass(LPCTSTR lpszNewValue);
	CString GetMileage();
	void SetMileage(LPCTSTR lpszNewValue);
	BOOL GetNoAging();
	void SetNoAging(BOOL bNewValue);
	long GetOutlookInternalVersion();
	CString GetOutlookVersion();
	BOOL GetSaved();
	long GetSensitivity();
	void SetSensitivity(long nNewValue);
	long GetSize();
	CString GetSubject();
	void SetSubject(LPCTSTR lpszNewValue);
	BOOL GetUnRead();
	void SetUnRead(BOOL bNewValue);
	LPDISPATCH GetUserProperties();
	void Close(long SaveMode);
	LPDISPATCH Copy();
	void Delete();
	void Display(const VARIANT& Modal);
	LPDISPATCH Move(LPDISPATCH DestFldr);
	void PrintOut();
	void Save();
	void SaveAs(LPCTSTR Path, const VARIANT& Type);
	CString GetDLName();
	void SetDLName(LPCTSTR lpszNewValue);
	long GetMemberCount();
	LPDISPATCH GetLinks();
	void AddMembers(LPDISPATCH Recipients);
	void RemoveMembers(LPDISPATCH Recipients);
	LPDISPATCH GetMember(long Index);
};
/////////////////////////////////////////////////////////////////////////////
// _JournalItem wrapper class

class _JournalItem : public COleDispatchDriver
{
public:
	_JournalItem() {}		// Calls COleDispatchDriver default constructor
	_JournalItem(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	_JournalItem(const _JournalItem& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	LPDISPATCH GetApplication();
	long GetClass();
	LPDISPATCH GetSession();
	LPDISPATCH GetParent();
	LPDISPATCH GetActions();
	LPDISPATCH GetAttachments();
	CString GetBillingInformation();
	void SetBillingInformation(LPCTSTR lpszNewValue);
	CString GetBody();
	void SetBody(LPCTSTR lpszNewValue);
	CString GetCategories();
	void SetCategories(LPCTSTR lpszNewValue);
	CString GetCompanies();
	void SetCompanies(LPCTSTR lpszNewValue);
	CString GetConversationIndex();
	CString GetConversationTopic();
	DATE GetCreationTime();
	CString GetEntryID();
	LPDISPATCH GetFormDescription();
	LPDISPATCH GetGetInspector();
	long GetImportance();
	void SetImportance(long nNewValue);
	DATE GetLastModificationTime();
	CString GetMessageClass();
	void SetMessageClass(LPCTSTR lpszNewValue);
	CString GetMileage();
	void SetMileage(LPCTSTR lpszNewValue);
	BOOL GetNoAging();
	void SetNoAging(BOOL bNewValue);
	long GetOutlookInternalVersion();
	CString GetOutlookVersion();
	BOOL GetSaved();
	long GetSensitivity();
	void SetSensitivity(long nNewValue);
	long GetSize();
	CString GetSubject();
	void SetSubject(LPCTSTR lpszNewValue);
	BOOL GetUnRead();
	void SetUnRead(BOOL bNewValue);
	LPDISPATCH GetUserProperties();
	void Close(long SaveMode);
	LPDISPATCH Copy();
	void Delete();
	void Display(const VARIANT& Modal);
	LPDISPATCH Move(LPDISPATCH DestFldr);
	void PrintOut();
	void Save();
	void SaveAs(LPCTSTR Path, const VARIANT& Type);
	CString GetContactNames();
	void SetContactNames(LPCTSTR lpszNewValue);
	BOOL GetDocPosted();
	void SetDocPosted(BOOL bNewValue);
	BOOL GetDocPrinted();
	void SetDocPrinted(BOOL bNewValue);
	BOOL GetDocRouted();
	void SetDocRouted(BOOL bNewValue);
	BOOL GetDocSaved();
	void SetDocSaved(BOOL bNewValue);
	long GetDuration();
	void SetDuration(long nNewValue);
	DATE GetEnd();
	void SetEnd(DATE newValue);
	CString GetType();
	void SetType(LPCTSTR lpszNewValue);
	LPDISPATCH GetRecipients();
	DATE GetStart();
	void SetStart(DATE newValue);
	LPDISPATCH Forward();
	LPDISPATCH Reply();
	LPDISPATCH ReplyAll();
	void StartTimer();
	void StopTimer();
	LPDISPATCH GetLinks();
};
/////////////////////////////////////////////////////////////////////////////
// _NoteItem wrapper class

class _NoteItem : public COleDispatchDriver
{
public:
	_NoteItem() {}		// Calls COleDispatchDriver default constructor
	_NoteItem(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	_NoteItem(const _NoteItem& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	LPDISPATCH GetApplication();
	long GetClass();
	LPDISPATCH GetSession();
	LPDISPATCH GetParent();
	CString GetBody();
	void SetBody(LPCTSTR lpszNewValue);
	CString GetCategories();
	void SetCategories(LPCTSTR lpszNewValue);
	long GetColor();
	void SetColor(long nNewValue);
	DATE GetCreationTime();
	CString GetEntryID();
	LPDISPATCH GetGetInspector();
	long GetHeight();
	void SetHeight(long nNewValue);
	DATE GetLastModificationTime();
	long GetLeft();
	void SetLeft(long nNewValue);
	CString GetMessageClass();
	void SetMessageClass(LPCTSTR lpszNewValue);
	BOOL GetSaved();
	long GetSize();
	CString GetSubject();
	long GetTop();
	void SetTop(long nNewValue);
	long GetWidth();
	void SetWidth(long nNewValue);
	void Close(long SaveMode);
	LPDISPATCH Copy();
	void Delete();
	void Display(const VARIANT& Modal);
	LPDISPATCH Move(LPDISPATCH DestFldr);
	void PrintOut();
	void Save();
	void SaveAs(LPCTSTR Path, const VARIANT& Type);
	LPDISPATCH GetLinks();
};
/////////////////////////////////////////////////////////////////////////////
// Action wrapper class

class Action : public COleDispatchDriver
{
public:
	Action() {}		// Calls COleDispatchDriver default constructor
	Action(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	Action(const Action& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	LPDISPATCH GetApplication();
	long GetClass();
	LPDISPATCH GetSession();
	LPDISPATCH GetParent();
	long GetCopyLike();
	void SetCopyLike(long nNewValue);
	BOOL GetEnabled();
	void SetEnabled(BOOL bNewValue);
	CString GetMessageClass();
	void SetMessageClass(LPCTSTR lpszNewValue);
	CString GetName();
	void SetName(LPCTSTR lpszNewValue);
	CString GetPrefix();
	void SetPrefix(LPCTSTR lpszNewValue);
	long GetReplyStyle();
	void SetReplyStyle(long nNewValue);
	long GetResponseStyle();
	void SetResponseStyle(long nNewValue);
	long GetShowOn();
	void SetShowOn(long nNewValue);
	void Delete();
	LPDISPATCH Execute();
};
/////////////////////////////////////////////////////////////////////////////
// Recipient wrapper class

class Recipient : public COleDispatchDriver
{
public:
	Recipient() {}		// Calls COleDispatchDriver default constructor
	Recipient(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	Recipient(const Recipient& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	LPDISPATCH GetApplication();
	long GetClass();
	LPDISPATCH GetSession();
	LPDISPATCH GetParent();
	CString GetAddress();
	LPDISPATCH GetAddressEntry();
	void SetRefAddressEntry(LPDISPATCH newValue);
	CString GetAutoResponse();
	void SetAutoResponse(LPCTSTR lpszNewValue);
	long GetDisplayType();
	CString GetEntryID();
	long GetIndex();
	long GetMeetingResponseStatus();
	CString GetName();
	BOOL GetResolved();
	long GetTrackingStatus();
	void SetTrackingStatus(long nNewValue);
	DATE GetTrackingStatusTime();
	void SetTrackingStatusTime(DATE newValue);
	long GetType();
	void SetType(long nNewValue);
	void Delete();
	CString FreeBusy(DATE Start, long MinPerChar, const VARIANT& CompleteFormat);
	BOOL Resolve();
};
/////////////////////////////////////////////////////////////////////////////
// _Inspector wrapper class

class _Inspector : public COleDispatchDriver
{
public:
	_Inspector() {}		// Calls COleDispatchDriver default constructor
	_Inspector(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	_Inspector(const _Inspector& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	LPDISPATCH GetApplication();
	long GetClass();
	LPDISPATCH GetSession();
	LPDISPATCH GetParent();
	LPDISPATCH GetCommandBars();
	LPDISPATCH GetCurrentItem();
	long GetEditorType();
	LPDISPATCH GetModifiedFormPages();
	void Close(long SaveMode);
	void Display(const VARIANT& Modal);
	void HideFormPage(LPCTSTR PageName);
	BOOL IsWordMail();
	void SetCurrentFormPage(LPCTSTR PageName);
	void ShowFormPage(LPCTSTR PageName);
	LPDISPATCH GetHTMLEditor();
	LPDISPATCH GetWordEditor();
	CString GetCaption();
	long GetHeight();
	void SetHeight(long nNewValue);
	long GetLeft();
	void SetLeft(long nNewValue);
	long GetTop();
	void SetTop(long nNewValue);
	long GetWidth();
	void SetWidth(long nNewValue);
	long GetWindowState();
	void SetWindowState(long nNewValue);
	void Activate();
};
/////////////////////////////////////////////////////////////////////////////
// Actions wrapper class

class Actions : public COleDispatchDriver
{
public:
	Actions() {}		// Calls COleDispatchDriver default constructor
	Actions(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	Actions(const Actions& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	LPDISPATCH GetApplication();
	long GetClass();
	LPDISPATCH GetSession();
	LPDISPATCH GetParent();
	long GetCount();
	LPDISPATCH Item(const VARIANT& Index);
	LPDISPATCH Add();
	void Remove(long Index);
};
/////////////////////////////////////////////////////////////////////////////
// Attachments wrapper class

class Attachments : public COleDispatchDriver
{
public:
	Attachments() {}		// Calls COleDispatchDriver default constructor
	Attachments(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	Attachments(const Attachments& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	LPDISPATCH GetApplication();
	long GetClass();
	LPDISPATCH GetSession();
	LPDISPATCH GetParent();
	long GetCount();
	LPDISPATCH Item(const VARIANT& Index);
	LPDISPATCH Add(const VARIANT& Source, const VARIANT& Type, const VARIANT& Position, const VARIANT& DisplayName);
	void Remove(long Index);
};
/////////////////////////////////////////////////////////////////////////////
// Attachment wrapper class

class Attachment : public COleDispatchDriver
{
public:
	Attachment() {}		// Calls COleDispatchDriver default constructor
	Attachment(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	Attachment(const Attachment& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	LPDISPATCH GetApplication();
	long GetClass();
	LPDISPATCH GetSession();
	LPDISPATCH GetParent();
	CString GetDisplayName();
	void SetDisplayName(LPCTSTR lpszNewValue);
	CString GetFileName();
	long GetIndex();
	CString GetPathName();
	long GetPosition();
	void SetPosition(long nNewValue);
	long GetType();
	void Delete();
	void SaveAsFile(LPCTSTR Path);
};
/////////////////////////////////////////////////////////////////////////////
// FormDescription wrapper class

class FormDescription : public COleDispatchDriver
{
public:
	FormDescription() {}		// Calls COleDispatchDriver default constructor
	FormDescription(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	FormDescription(const FormDescription& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	LPDISPATCH GetApplication();
	long GetClass();
	LPDISPATCH GetSession();
	LPDISPATCH GetParent();
	CString GetCategory();
	void SetCategory(LPCTSTR lpszNewValue);
	CString GetCategorySub();
	void SetCategorySub(LPCTSTR lpszNewValue);
	CString GetComment();
	void SetComment(LPCTSTR lpszNewValue);
	CString GetContactName();
	void SetContactName(LPCTSTR lpszNewValue);
	CString GetDisplayName();
	void SetDisplayName(LPCTSTR lpszNewValue);
	BOOL GetHidden();
	void SetHidden(BOOL bNewValue);
	CString GetIcon();
	void SetIcon(LPCTSTR lpszNewValue);
	BOOL GetLocked();
	void SetLocked(BOOL bNewValue);
	CString GetMessageClass();
	CString GetMiniIcon();
	void SetMiniIcon(LPCTSTR lpszNewValue);
	CString GetName();
	void SetName(LPCTSTR lpszNewValue);
	CString GetNumber();
	void SetNumber(LPCTSTR lpszNewValue);
	BOOL GetOneOff();
	void SetOneOff(BOOL bNewValue);
	CString GetPassword();
	void SetPassword(LPCTSTR lpszNewValue);
	CString GetScriptText();
	CString GetTemplate();
	void SetTemplate(LPCTSTR lpszNewValue);
	BOOL GetUseWordMail();
	void SetUseWordMail(BOOL bNewValue);
	CString GetVersion();
	void SetVersion(LPCTSTR lpszNewValue);
	void PublishForm(long Registry, const VARIANT& Folder);
};
/////////////////////////////////////////////////////////////////////////////
// Recipients wrapper class

class Recipients : public COleDispatchDriver
{
public:
	Recipients() {}		// Calls COleDispatchDriver default constructor
	Recipients(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	Recipients(const Recipients& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	LPDISPATCH GetApplication();
	long GetClass();
	LPDISPATCH GetSession();
	LPDISPATCH GetParent();
	long GetCount();
	LPDISPATCH Item(const VARIANT& Index);
	LPDISPATCH Add(LPCTSTR Name);
	void Remove(long Index);
	BOOL ResolveAll();
};
/////////////////////////////////////////////////////////////////////////////
// Links wrapper class

class Links : public COleDispatchDriver
{
public:
	Links() {}		// Calls COleDispatchDriver default constructor
	Links(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	Links(const Links& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	LPDISPATCH GetApplication();
	long GetClass();
	LPDISPATCH GetSession();
	LPDISPATCH GetParent();
	long GetCount();
	LPDISPATCH Item(const VARIANT& Index);
	LPDISPATCH Add(LPDISPATCH Item);
	void Remove(const VARIANT& Index);
};
/////////////////////////////////////////////////////////////////////////////
// Link wrapper class

class Link : public COleDispatchDriver
{
public:
	Link() {}		// Calls COleDispatchDriver default constructor
	Link(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	Link(const Link& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	LPDISPATCH GetApplication();
	long GetClass();
	LPDISPATCH GetSession();
	LPDISPATCH GetParent();
	CString GetName();
	long GetType();
	LPDISPATCH GetItem();
};
/////////////////////////////////////////////////////////////////////////////
// RecurrencePattern wrapper class

class RecurrencePattern : public COleDispatchDriver
{
public:
	RecurrencePattern() {}		// Calls COleDispatchDriver default constructor
	RecurrencePattern(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	RecurrencePattern(const RecurrencePattern& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	LPDISPATCH GetApplication();
	long GetClass();
	LPDISPATCH GetSession();
	LPDISPATCH GetParent();
	long GetDayOfMonth();
	void SetDayOfMonth(long nNewValue);
	long GetDayOfWeekMask();
	void SetDayOfWeekMask(long nNewValue);
	long GetDuration();
	void SetDuration(long nNewValue);
	DATE GetEndTime();
	void SetEndTime(DATE newValue);
	LPDISPATCH GetExceptions();
	long GetInstance();
	void SetInstance(long nNewValue);
	long GetInterval();
	void SetInterval(long nNewValue);
	long GetMonthOfYear();
	void SetMonthOfYear(long nNewValue);
	BOOL GetNoEndDate();
	void SetNoEndDate(BOOL bNewValue);
	long GetOccurrences();
	void SetOccurrences(long nNewValue);
	DATE GetPatternEndDate();
	void SetPatternEndDate(DATE newValue);
	DATE GetPatternStartDate();
	void SetPatternStartDate(DATE newValue);
	long GetRecurrenceType();
	void SetRecurrenceType(long nNewValue);
	BOOL GetRegenerate();
	void SetRegenerate(BOOL bNewValue);
	DATE GetStartTime();
	void SetStartTime(DATE newValue);
	LPDISPATCH GetOccurrence(DATE StartDate);
};

/////////////////////////////////////////////////////////////////////////////
// UserProperties wrapper class

class UserProperties : public COleDispatchDriver
{
public:
	UserProperties() {}		// Calls COleDispatchDriver default constructor
	UserProperties(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	UserProperties(const UserProperties& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	LPDISPATCH GetApplication();
	long GetClass();
	LPDISPATCH GetSession();
	LPDISPATCH GetParent();
	long GetCount();
	LPDISPATCH Item(const VARIANT& Index);
	LPDISPATCH Add(LPCTSTR Name, long Type, const VARIANT& AddToFolderFields, const VARIANT& DisplayFormat);
	LPDISPATCH Find(LPCTSTR Name, const VARIANT& Custom);
	void Remove(long Index);
};
/////////////////////////////////////////////////////////////////////////////
// Exceptions wrapper class

class Exceptions : public COleDispatchDriver
{
public:
	Exceptions() {}		// Calls COleDispatchDriver default constructor
	Exceptions(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	Exceptions(const Exceptions& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	LPDISPATCH GetApplication();
	long GetClass();
	LPDISPATCH GetSession();
	LPDISPATCH GetParent();
	long GetCount();
	LPDISPATCH Item(const VARIANT& Index);
};
/////////////////////////////////////////////////////////////////////////////
// Exception wrapper class

class Exception : public COleDispatchDriver
{
public:
	Exception() {}		// Calls COleDispatchDriver default constructor
	Exception(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	Exception(const Exception& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	LPDISPATCH GetApplication();
	long GetClass();
	LPDISPATCH GetSession();
	LPDISPATCH GetParent();
	LPDISPATCH GetAppointmentItem();
	BOOL GetDeleted();
	DATE GetOriginalDate();
};
#endif
