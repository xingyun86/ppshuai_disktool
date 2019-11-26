
// DiskToolDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "DiskTool.h"
#include "DiskToolDlg.h"
#include "afxdialogex.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CDiskToolDlg dialog



CDiskToolDlg::CDiskToolDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DISKTOOL_DIALOG, pParent)
{
	hVolume = INVALID_HANDLE_VALUE;
	hFile = INVALID_HANDLE_VALUE;
	hRawDisk = INVALID_HANDLE_VALUE;
	sectorsize = 0LL;
	sectorData = nullptr;
	sectorData2 = nullptr; //for verify
	status = STATUS_IDLE;
	m_FilePath = _T("");
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDiskToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDiskToolDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DEVICECHANGE()
	ON_BN_CLICKED(IDC_BUTTON1, &CDiskToolDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CDiskToolDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CDiskToolDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CDiskToolDlg::OnBnClickedButton4)
END_MESSAGE_MAP()


// CDiskToolDlg message handlers

BOOL CDiskToolDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	{
		GetAllLogicalDrives(m_device_map);
		status = STATUS_IDLE;
		for (auto& it : m_device_map)
		{
			((CComboBox*)GetDlgItem(IDC_COMBO1))->InsertString(((CComboBox*)GetDlgItem(IDC_COMBO1))->GetCount(), CString(it.first.c_str()));
		}
		((CComboBox*)GetDlgItem(IDC_COMBO1))->SetCurSel(0);

		return FALSE;
		
	}
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDiskToolDlg::OnCancel()
{
	if (hRawDisk != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hRawDisk);
		hRawDisk = INVALID_HANDLE_VALUE;
	}
	if (hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile);
		hFile = INVALID_HANDLE_VALUE;
	}
	if (hVolume != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hVolume);
		hVolume = INVALID_HANDLE_VALUE;
	}
	if (sectorData != NULL)
	{
		delete[] sectorData;
		sectorData = NULL;
	}
	if (sectorData2 != NULL)
	{
		delete[] sectorData2;
		sectorData2 = NULL;
	}

	CDialogEx::OnCancel();
}

void CDiskToolDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDiskToolDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDiskToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CDiskToolDlg::OnDeviceChange(UINT nEventType, DWORD dwData)
{
	DEV_BROADCAST_HDR * pdbd = (DEV_BROADCAST_HDR*)dwData;
	//DEV_BROADCAST_DEVICEINTERFACE* pdbd = (DEV_BROADCAST_DEVICEINTERFACE*)dwData;
	//
	//这里进行信息匹配,比如guid等
	switch (nEventType)
	{
	case DBT_DEVICEARRIVAL:
	{
		::AfxMessageBox(TEXT("得到新的设备"), 1, 0);
		if (pdbd->dbch_devicetype == DBT_DEVTYP_VOLUME)
		{
			PDEV_BROADCAST_VOLUME lpdbv = (PDEV_BROADCAST_VOLUME)pdbd;
			if (DBTF_NET)
			{
				char ALET = FirstDriveFromMask(lpdbv->dbcv_unitmask);
				// add device to combo box (after sanity check that
				// it's not already there, which it shouldn't be)
				CString drive = ("");
				drive.Format("[%C:\\]", ALET);
				
				if (m_device_map.find((const char *)CStringA(drive)) == m_device_map.end())
				{
					ULONG pID;
					char drivename[] = "\\\\.\\A:\\";
					drivename[4] = ALET;
					// checkDriveType gets the physicalID
					if (CheckDriveType(drivename, &pID))
					{
						//cboxDevice->addItem(qs, (qulonglong)pID);
						m_device_map.insert(std::map<std::string, ULONG>::value_type("[" + std::string(&drivename[4], sizeof(char)) + ":\\]", pID));
					}
				}
			}
		}
	}
	return TRUE;
	case DBT_DEVICEREMOVECOMPLETE:
	{
		if (pdbd->dbch_devicetype == DBT_DEVTYP_VOLUME)
		{
			PDEV_BROADCAST_VOLUME lpdbv = (PDEV_BROADCAST_VOLUME)pdbd;
			if (DBTF_NET)
			{
				char ALET = FirstDriveFromMask(lpdbv->dbcv_unitmask);
				//  find the device that was removed in the combo box,
				//  and remove it from there....
				//  "removeItem" ignores the request if the index is
				//  out of range, and findText returns -1 if the item isn't found.
				CString drive = ("");
				drive.Format("[%C:\\]", ALET);
				if (m_device_map.find((const char*)CStringA(drive)) != m_device_map.end())
				{
					m_device_map.erase((const char*)CStringA(drive));
				}
			}
		}
	}
	return TRUE;
	}
	return FALSE;
	/*
	tips：其中nEventType就是WM_DEVICECHANGE消息的wParam参数，具体的值参考msdn，具体的含义直接查就好了。

	DBT_CONFIGCHANGECANCELED
	DBT_CONFIGCHANGED
	DBT_CUSTOMEVENT
	DBT_DEVICEARRIVAL
	DBT_DEVICEQUERYREMOVE
	DBT_DEVICEQUERYREMOVEFAILED
	DBT_DEVICEREMOVECOMPLETE
	DBT_DEVICEREMOVEPENDING
	DBT_DEVICETYPESPECIFIC
	DBT_DEVNODES_CHANGED
	DBT_QUERYCHANGECONFIG
	DBT_USERDEFINED
	*/
}

//Open img
void CDiskToolDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here

	CFileDialog findFileDlg(TRUE,  // TRUE是创建打开文件对话框，FALSE则创建的是保存文件对话框
		".img",  // 默认的打开文件的类型
		NULL,  // 默认打开的文件名
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,  // 打开只读文件
		"镜像文件(*.img)|*.IMG|所有文件 (*.*)|*.*||"  // 所有可以打开的文件类型
	);
	if (IDOK == findFileDlg.DoModal())
	{
		m_FilePath = _T("");
	}
	else
	{
		m_FilePath = findFileDlg.GetPathName();  // 取出文件路径 
	}
}

//Verify
void CDiskToolDlg::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
	char drive='\0';
	CString strDriver(_T(""));
	int nCurSel = ((CComboBox*)GetDlgItem(IDC_COMBO1))->GetCurSel();
	if (nCurSel == (-1))
	{
		return;
	}
	((CComboBox*)GetDlgItem(IDC_COMBO1))->GetLBText(nCurSel, strDriver);
	if (strDriver.GetLength() <= 1 || MessageBox(strDriver, _T("Verify"), MB_OKCANCEL) == IDCANCEL)
	{
		return;
	}
	drive = ((const char *)CStringA(strDriver))[1];
	UINT64 i, lasti, availablesectors, numsectors, result;
	int volumeID = drive - 'A';
	hVolume = GetHandleOnVolume(volumeID, GENERIC_READ);
	if (hVolume == INVALID_HANDLE_VALUE)
	{
		status = STATUS_IDLE;
		return;
	}
	DWORD deviceID = GetDeviceID(hVolume);
	if (!GetLockOnVolume(hVolume))
	{
		CloseHandle(hVolume);
		status = STATUS_IDLE;
		hVolume = INVALID_HANDLE_VALUE;
		return;
	}
	if (!UnmountVolume(hVolume))
	{
		RemoveLockOnVolume(hVolume);
		CloseHandle(hVolume);
		status = STATUS_IDLE;
		hVolume = INVALID_HANDLE_VALUE;
		return;
	}
	hFile = GetHandleOnFile(LPCWSTR(CStringW(m_FilePath)), GENERIC_READ);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		RemoveLockOnVolume(hVolume);
		CloseHandle(hVolume);
		status = STATUS_IDLE;
		hVolume = INVALID_HANDLE_VALUE;
		return;
	}
	hRawDisk = GetHandleOnDevice(deviceID, GENERIC_READ);
	if (hRawDisk == INVALID_HANDLE_VALUE)
	{
		RemoveLockOnVolume(hVolume);
		CloseHandle(hFile);
		CloseHandle(hVolume);
		status = STATUS_IDLE;
		hVolume = INVALID_HANDLE_VALUE;
		hFile = INVALID_HANDLE_VALUE;
		return;
	}
	availablesectors = GetNumberOfSectors(hRawDisk, &sectorsize);
	if (!availablesectors)
	{
		//For external card readers you may not get device change notification when you remove the card/flash.
		//(So no WM_DEVICECHANGE signal). Device stays but size goes to 0. [Is there special event for this on Windows??]
		RemoveLockOnVolume(hVolume);
		CloseHandle(hRawDisk);
		CloseHandle(hFile);
		CloseHandle(hVolume);
		hRawDisk = INVALID_HANDLE_VALUE;
		hFile = INVALID_HANDLE_VALUE;
		hVolume = INVALID_HANDLE_VALUE;
		status = STATUS_IDLE;
		return;

	}
	numsectors = GetFileSizeInSectors(hFile, sectorsize);
	if (!numsectors)
	{
		//For external card readers you may not get device change notification when you remove the card/flash.
		//(So no WM_DEVICECHANGE signal). Device stays but size goes to 0. [Is there special event for this on Windows??]
		RemoveLockOnVolume(hVolume);
		CloseHandle(hRawDisk);
		CloseHandle(hFile);
		CloseHandle(hVolume);
		hRawDisk = INVALID_HANDLE_VALUE;
		hFile = INVALID_HANDLE_VALUE;
		hVolume = INVALID_HANDLE_VALUE;
		status = STATUS_IDLE;
		return;

	}
	if (numsectors > availablesectors)
	{
		bool datafound = false;
		i = availablesectors;
		unsigned long nextchunksize = 0;
		while ((i < numsectors) && (datafound == false))
		{
			nextchunksize = ((numsectors - i) >= 1024ul) ? 1024ul : (numsectors - i);
			sectorData = ReadSectorDataFromHandle(hFile, i, nextchunksize, sectorsize);
			if (sectorData == NULL)
			{
				// if there's an error verifying the truncated data, just move on to the
				//  write, as we don't care about an error in a section that we're not writing...
				i = numsectors + 1;
			}
			else {
				unsigned int j = 0;
				unsigned limit = nextchunksize * sectorsize;
				while ((datafound == false) && (j < limit))
				{
					if (sectorData[j++] != 0)
					{
						datafound = true;
					}
				}
				i += nextchunksize;
			}
		}
		if (sectorData != nullptr)
		{
			// delete the allocated sectorData
			delete[] sectorData;
			sectorData = NULL;
		}
		// build the string for the warning dialog
		std::string msg;
		msg.append("Size of image larger than device:")
			.append("\n  Image: ").append(std::to_string(numsectors)).append(" sectors")
			.append("\n  Device: ").append(std::to_string(availablesectors)).append(" sectors")
			.append("\n  Sector Size: ").append(std::to_string(sectorsize))
			.append("\n\nThe extra space ").append(((datafound) ? "DOES" : "does not")).append(" appear to contain data")
			.append("\n\nContinue Anyway?");
		if (MessageBox(_T("Size Mismatch!"), msg.c_str(), MB_OKCANCEL) == IDOK)
		{
			// truncate the image at the device size...
			numsectors = availablesectors;
		}
		else    // Cancel
		{
			RemoveLockOnVolume(hVolume);
			CloseHandle(hRawDisk);
			CloseHandle(hFile);
			CloseHandle(hVolume);
			status = STATUS_IDLE;
			hVolume = INVALID_HANDLE_VALUE;
			hFile = INVALID_HANDLE_VALUE;
			hRawDisk = INVALID_HANDLE_VALUE;
			return;
		}
	}

	lasti = 0ul;
	for (i = 0ul; i < numsectors && status == STATUS_VERIFYING; i += 1024ul)
	{
		sectorData = ReadSectorDataFromHandle(hFile, i, (numsectors - i >= 1024ul) ? 1024ul : (numsectors - i), sectorsize);
		if (sectorData == NULL)
		{
			RemoveLockOnVolume(hVolume);
			CloseHandle(hRawDisk);
			CloseHandle(hFile);
			CloseHandle(hVolume);
			status = STATUS_IDLE;
			hRawDisk = INVALID_HANDLE_VALUE;
			hFile = INVALID_HANDLE_VALUE;
			hVolume = INVALID_HANDLE_VALUE;
			return;
		}
		sectorData2 = ReadSectorDataFromHandle(hRawDisk, i, (numsectors - i >= 1024ul) ? 1024ul : (numsectors - i), sectorsize);
		if (sectorData2 == NULL)
		{
			RemoveLockOnVolume(hVolume);
			CloseHandle(hRawDisk);
			CloseHandle(hFile);
			CloseHandle(hVolume);
			status = STATUS_IDLE;
			hRawDisk = INVALID_HANDLE_VALUE;
			hFile = INVALID_HANDLE_VALUE;
			hVolume = INVALID_HANDLE_VALUE;
			return;
		}
		result = memcmp(sectorData, sectorData2, ((numsectors - i >= 1024ul) ? 1024ul : (numsectors - i)) * sectorsize);
		if (result)
		{
			break;
		}
		delete[] sectorData;
		delete[] sectorData2;
		sectorData = NULL;
		sectorData2 = NULL;
	}
	RemoveLockOnVolume(hVolume);
	CloseHandle(hRawDisk);
	CloseHandle(hFile);
	CloseHandle(hVolume);
	if (sectorData != nullptr)
	{
		delete[] sectorData;
	}
	if (sectorData2 != nullptr)
	{
		delete[] sectorData2;
	}
	sectorData = NULL;
	sectorData2 = NULL;
	hRawDisk = INVALID_HANDLE_VALUE;
	hFile = INVALID_HANDLE_VALUE;
	hVolume = INVALID_HANDLE_VALUE;
}

//Read
void CDiskToolDlg::OnBnClickedButton3()
{
	// TODO: Add your control notification handler code here
	char drive = '\0';
	CString strDriver(_T(""));
	int nCurSel = ((CComboBox*)GetDlgItem(IDC_COMBO1))->GetCurSel();
	if (nCurSel == (-1))
	{
		return;
	}
	((CComboBox*)GetDlgItem(IDC_COMBO1))->GetLBText(nCurSel, strDriver);
	if (strDriver.GetLength() <= 1 || MessageBox(strDriver, _T("Read"), MB_OKCANCEL) == IDCANCEL)
	{
		return;
	}
	drive = ((const char*)CStringA(strDriver))[1];
	status = STATUS_READING;
	double mbpersec;
	UINT64 i, lasti, numsectors, filesize, spaceneeded = 0ull;
	int volumeID = drive - 'A';
	hVolume = GetHandleOnVolume(volumeID, GENERIC_READ);
	if (hVolume == INVALID_HANDLE_VALUE)
	{
		status = STATUS_IDLE;
		return;
	}
	DWORD deviceID = GetDeviceID(hVolume);
	if (!GetLockOnVolume(hVolume))
	{
		CloseHandle(hVolume);
		status = STATUS_IDLE;
		hVolume = INVALID_HANDLE_VALUE;
		return;
	}
	if (!UnmountVolume(hVolume))
	{
		RemoveLockOnVolume(hVolume);
		CloseHandle(hVolume);
		status = STATUS_IDLE;
		hVolume = INVALID_HANDLE_VALUE;
		return;
	}
	hFile = GetHandleOnFile(LPCWSTR(""), GENERIC_WRITE);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		RemoveLockOnVolume(hVolume);
		CloseHandle(hVolume);
		status = STATUS_IDLE;
		hVolume = INVALID_HANDLE_VALUE;
		return;
	}
	hRawDisk = GetHandleOnDevice(deviceID, GENERIC_READ);
	if (hRawDisk == INVALID_HANDLE_VALUE)
	{
		RemoveLockOnVolume(hVolume);
		CloseHandle(hFile);
		CloseHandle(hVolume);
		status = STATUS_IDLE;
		hVolume = INVALID_HANDLE_VALUE;
		hFile = INVALID_HANDLE_VALUE;
		return;
	}
	numsectors = GetNumberOfSectors(hRawDisk, &sectorsize);
	bool bPartitionCheck = true;
	if (bPartitionCheck == true)
	{
		// Read MBR partition table
		sectorData = ReadSectorDataFromHandle(hRawDisk, 0, 1ul, 512ul);
		numsectors = 1ul;
		// Read partition information
		for (i = 0ul; i < 4ul; i++)
		{
			uint32_t partitionStartSector = *((uint32_t*)(sectorData + 0x1BE + 8 + 16 * i));
			uint32_t partitionNumSectors = *((uint32_t*)(sectorData + 0x1BE + 12 + 16 * i));
			// Set numsectors to end of last partition
			if (partitionStartSector + partitionNumSectors > numsectors)
			{
				numsectors = partitionStartSector + partitionNumSectors;
			}
		}
	}
	filesize = GetFileSizeInSectors(hFile, sectorsize);
	if (filesize >= numsectors)
	{
		spaceneeded = 0ull;
	}
	else
	{
		spaceneeded = (unsigned long long)(numsectors - filesize) * (unsigned long long)(sectorsize);
	}
	if (!SpaceAvailable("", spaceneeded))
	{
		//QMessageBox::critical(this, tr("Write Error"), tr("Disk is not large enough for the specified image."));
		RemoveLockOnVolume(hVolume);
		CloseHandle(hRawDisk);
		CloseHandle(hFile);
		CloseHandle(hVolume);
		status = STATUS_IDLE;
		sectorData = NULL;
		hRawDisk = INVALID_HANDLE_VALUE;
		hFile = INVALID_HANDLE_VALUE;
		hVolume = INVALID_HANDLE_VALUE;
		return;
	}
	lasti = 0ul;
	for (i = 0ul; i < numsectors && status == STATUS_READING; i += 1024ul)
	{
		sectorData = ReadSectorDataFromHandle(hRawDisk, i, (numsectors - i >= 1024ul) ? 1024ul : (numsectors - i), sectorsize);
		if (sectorData == NULL)
		{
			RemoveLockOnVolume(hVolume);
			CloseHandle(hRawDisk);
			CloseHandle(hFile);
			CloseHandle(hVolume);
			status = STATUS_IDLE;
			hRawDisk = INVALID_HANDLE_VALUE;
			hFile = INVALID_HANDLE_VALUE;
			hVolume = INVALID_HANDLE_VALUE;
			return;
		}
		if (!WriteSectorDataToHandle(hFile, sectorData, i, (numsectors - i >= 1024ul) ? 1024ul : (numsectors - i), sectorsize))
		{
			delete[] sectorData;
			RemoveLockOnVolume(hVolume);
			CloseHandle(hRawDisk);
			CloseHandle(hFile);
			CloseHandle(hVolume);
			status = STATUS_IDLE;
			sectorData = NULL;
			hRawDisk = INVALID_HANDLE_VALUE;
			hFile = INVALID_HANDLE_VALUE;
			hVolume = INVALID_HANDLE_VALUE;
			return;
		}
		delete[] sectorData;
		sectorData = NULL;
	}
	RemoveLockOnVolume(hVolume);
	CloseHandle(hRawDisk);
	CloseHandle(hFile);
	CloseHandle(hVolume);
	hRawDisk = INVALID_HANDLE_VALUE;
	hFile = INVALID_HANDLE_VALUE;
	hVolume = INVALID_HANDLE_VALUE;
}

//Write
void CDiskToolDlg::OnBnClickedButton4()
{
	// TODO: Add your control notification handler code here
	char drive = '\0';
	CString strDriver(_T(""));
	int nCurSel = ((CComboBox*)GetDlgItem(IDC_COMBO1))->GetCurSel();
	if (nCurSel == (-1))
	{
		return;
	}
	((CComboBox*)GetDlgItem(IDC_COMBO1))->GetLBText(nCurSel, strDriver);
	if (strDriver.GetLength() <= 1 || MessageBox(strDriver, _T("Write"), MB_OKCANCEL) == IDCANCEL)
	{
		return;
	}
	drive = ((const char*)CStringA(strDriver))[1];
	double mbpersec;
	unsigned long long i, lasti, availablesectors, numsectors;
	int volumeID = drive - 'A';
	// int deviceID = cboxDevice->itemData(cboxDevice->currentIndex()).toInt();
	hVolume = GetHandleOnVolume(volumeID, GENERIC_WRITE);
	if (hVolume == INVALID_HANDLE_VALUE)
	{
		status = STATUS_IDLE;
		return;
	}
	DWORD deviceID = GetDeviceID(hVolume);
	if (!GetLockOnVolume(hVolume))
	{
		CloseHandle(hVolume);
		status = STATUS_IDLE;
		hVolume = INVALID_HANDLE_VALUE;
		return;
	}
	if (!UnmountVolume(hVolume))
	{
		RemoveLockOnVolume(hVolume);
		CloseHandle(hVolume);
		status = STATUS_IDLE;
		hVolume = INVALID_HANDLE_VALUE;
		return;
	}
	hFile = GetHandleOnFile(LPCWSTR(CStringW(m_FilePath)), GENERIC_READ);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		RemoveLockOnVolume(hVolume);
		CloseHandle(hVolume);
		status = STATUS_IDLE;
		hVolume = INVALID_HANDLE_VALUE;
		return;
	}
	hRawDisk = GetHandleOnDevice(deviceID, GENERIC_WRITE);
	if (hRawDisk == INVALID_HANDLE_VALUE)
	{
		RemoveLockOnVolume(hVolume);
		CloseHandle(hFile);
		CloseHandle(hVolume);
		status = STATUS_IDLE;
		hVolume = INVALID_HANDLE_VALUE;
		hFile = INVALID_HANDLE_VALUE;
		return;
	}
	availablesectors = GetNumberOfSectors(hRawDisk, &sectorsize);
	if (!availablesectors)
	{
		//For external card readers you may not get device change notification when you remove the card/flash.
		//(So no WM_DEVICECHANGE signal). Device stays but size goes to 0. [Is there special event for this on Windows??]
		RemoveLockOnVolume(hVolume);
		CloseHandle(hRawDisk);
		CloseHandle(hFile);
		CloseHandle(hVolume);
		hRawDisk = INVALID_HANDLE_VALUE;
		hFile = INVALID_HANDLE_VALUE;
		hVolume = INVALID_HANDLE_VALUE;
		status = STATUS_IDLE;
		return;

	}
	numsectors = GetFileSizeInSectors(hFile, sectorsize);
	if (!numsectors)
	{
		//For external card readers you may not get device change notification when you remove the card/flash.
		//(So no WM_DEVICECHANGE signal). Device stays but size goes to 0. [Is there special event for this on Windows??]
		RemoveLockOnVolume(hVolume);
		CloseHandle(hRawDisk);
		CloseHandle(hFile);
		CloseHandle(hVolume);
		hRawDisk = INVALID_HANDLE_VALUE;
		hFile = INVALID_HANDLE_VALUE;
		hVolume = INVALID_HANDLE_VALUE;
		status = STATUS_IDLE;
		return;
	}
	if (numsectors > availablesectors)
	{
		bool datafound = false;
		i = availablesectors;
		unsigned long nextchunksize = 0;
		while ((i < numsectors) && (datafound == false))
		{
			nextchunksize = ((numsectors - i) >= 1024ul) ? 1024ul : (numsectors - i);
			sectorData = ReadSectorDataFromHandle(hFile, i, nextchunksize, sectorsize);
			if (sectorData == NULL)
			{
				// if there's an error verifying the truncated data, just move on to the
				//  write, as we don't care about an error in a section that we're not writing...
				i = numsectors + 1;
			}
			else {
				unsigned int j = 0;
				unsigned limit = nextchunksize * sectorsize;
				while ((datafound == false) && (j < limit))
				{
					if (sectorData[j++] != 0)
					{
						datafound = true;
					}
				}
				i += nextchunksize;
			}
		}
		// delete the allocated sectorData
		delete[] sectorData;
		sectorData = NULL;
		// build the string for the warning dialog
		std::string msg;
		msg.append("More space required than is available:")
			.append("\n  Required: ").append(std::to_string(numsectors)).append(" sectors")
			.append("\n  Available: ").append(std::to_string(availablesectors)).append(" sectors")
			.append("\n  Sector Size: ").append(std::to_string(sectorsize))
			.append("\n\nThe extra space ").append(((datafound) ? "DOES" : "does not")).append(" appear to contain data")
			.append("\n\nContinue Anyway?");
		if (MessageBox(_T("Not enough available space!"), msg.c_str(), MB_OKCANCEL) == IDOK)
		{
			// truncate the image at the device size...
			numsectors = availablesectors;
		}
		else    // Cancel
		{
			RemoveLockOnVolume(hVolume);
			CloseHandle(hRawDisk);
			CloseHandle(hFile);
			CloseHandle(hVolume);
			status = STATUS_IDLE;
			hVolume = INVALID_HANDLE_VALUE;
			hFile = INVALID_HANDLE_VALUE;
			hRawDisk = INVALID_HANDLE_VALUE;
			return;
		}
	}

	lasti = 0ul;
	for (i = 0ul; i < numsectors && status == STATUS_WRITING; i += 1024ul)
	{
		sectorData = ReadSectorDataFromHandle(hFile, i, (numsectors - i >= 1024ul) ? 1024ul : (numsectors - i), sectorsize);
		if (sectorData == NULL)
		{
			RemoveLockOnVolume(hVolume);
			CloseHandle(hRawDisk);
			CloseHandle(hFile);
			CloseHandle(hVolume);
			status = STATUS_IDLE;
			hRawDisk = INVALID_HANDLE_VALUE;
			hFile = INVALID_HANDLE_VALUE;
			hVolume = INVALID_HANDLE_VALUE;
			return;
		}
		if (!WriteSectorDataToHandle(hRawDisk, sectorData, i, (numsectors - i >= 1024ul) ? 1024ul : (numsectors - i), sectorsize))
		{
			delete[] sectorData;
			RemoveLockOnVolume(hVolume);
			CloseHandle(hRawDisk);
			CloseHandle(hFile);
			CloseHandle(hVolume);
			status = STATUS_IDLE;
			sectorData = NULL;
			hRawDisk = INVALID_HANDLE_VALUE;
			hFile = INVALID_HANDLE_VALUE;
			hVolume = INVALID_HANDLE_VALUE;
			return;
		}
		delete[] sectorData;
		sectorData = NULL;
	}
	RemoveLockOnVolume(hVolume);
	CloseHandle(hRawDisk);
	CloseHandle(hFile);
	CloseHandle(hVolume);
	hRawDisk = INVALID_HANDLE_VALUE;
	hFile = INVALID_HANDLE_VALUE;
	hVolume = INVALID_HANDLE_VALUE;
}
