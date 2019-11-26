
// DiskToolDlg.h : header file
//

#pragma once


// CDiskToolDlg dialog
class CDiskToolDlg : public CDialogEx
{
// Construction
public:
	CDiskToolDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DISKTOOL_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	HANDLE hVolume;
	HANDLE hFile;
	HANDLE hRawDisk;
	UINT64 sectorsize;
	char* sectorData;
	char* sectorData2; //for verify
	Status status;
	CString m_FilePath;
	std::map<std::string, ULONG> m_device_map;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg BOOL OnDeviceChange(UINT nEventType, DWORD dwData);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	DECLARE_MESSAGE_MAP()

};
