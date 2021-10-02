// SmartPointerTest.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include "pch.h"

class DataClass
{
public:
	DataClass() {}
	~DataClass() {}
	int Value = 1;
	int Sum (int data) const
	{
		return data + Value + HiddenValue;
	}

private:
	int HiddenValue = 2;
};


class BaseObj
{
public:
	BaseObj(const BaseObj&) = delete;
	BaseObj& operator=(const BaseObj&) = delete;
	BaseObj(BaseObj&&) = default;
	BaseObj& operator=(BaseObj&&) = default;
	BaseObj() = delete;
	BaseObj(DataClass& data) : Data(data) {}
	virtual ~BaseObj() = 0;

protected:
	DataClass& Data;
};
BaseObj::~BaseObj() {}


class Hoge
	: public BaseObj
{
public:
	Hoge(const Hoge&) = delete;
	Hoge& operator=(const Hoge&) = delete;
	Hoge(Hoge&&) = default;
	Hoge& operator=(Hoge&&) = default;
	Hoge() = delete;
	Hoge(DataClass& data) : BaseObj(data) {}
	~Hoge() override {}
	
	bool GetFlag(bool flg)
	{
		return !flg;
	}
	
	int GetValue()
	{
		Result = Data.Sum(5);
		return Result;
	}

private:
	int Result = 0;
};


class Piyo
	: public BaseObj
{
public:
	Piyo(const Piyo&) = delete;
	Piyo& operator=(const Piyo&) = delete;
	Piyo(Piyo&&) = default;
	Piyo& operator=(Piyo&&) = default;
	Piyo() = delete;
	Piyo(DataClass& data) : BaseObj(data) {}
	~Piyo() override {}
	int TestFunc()
	{
		Result = Data.Sum(10) * 20;
		return Result;
	}

private:
	int Result = 0;
};

enum class ObjectType
{
	hoge,
	piyo,
	Max,
};

class Manager
{
public:
	Manager(const Manager&) = delete;
	Manager& operator=(const Manager&) = delete;
	Manager(Manager&&) = default;
	Manager& operator=(Manager&&) = default;
	Manager() = delete;
	~Manager() {}

	std::weak_ptr<Hoge> HogeObject;
	std::weak_ptr<Piyo> PiyoObject;

	Manager(DataClass& data) : Data(data)
	{
		const auto len = static_cast<int>(ObjectType::Max);
		ObjectList.reserve(len);
		ObjectList.resize(len);
		for (size_t i = 0; i < len; i++)
			ObjectList.push_back(std::shared_ptr<BaseObj>());
	}

	void GeneratorObject(ObjectType type)
	{
		ReleaseObject();

		switch (type)
		{
		case ObjectType::hoge:
			ObjectList[static_cast<int>(type)].reset(new Hoge(Data));
			HogeObject = std::static_pointer_cast<Hoge>(ObjectList[static_cast<int>(type)]);
			break;
		case ObjectType::piyo:
			ObjectList[static_cast<int>(type)].reset(new Piyo(Data));
			PiyoObject = std::static_pointer_cast<Piyo>(ObjectList[static_cast<int>(type)]);
			break;
		default:
			throw std::runtime_error("rang out");
		}
	}

	void ReleaseObject()
	{
		for (auto& item : ObjectList)
			if (item)
				item.reset();
	}

	template <class T>
	std::shared_ptr<T> GetObject(std::weak_ptr<T> obj)
	{
		auto prt = obj.lock();
		if (!prt)
			throw std::runtime_error("Non instance.");
		return prt;
	}

private:
	DataClass& Data;
	std::vector<std::shared_ptr<BaseObj>> ObjectList;
};

// 関数呼び出しだけならこの方法でok 戻り値も取得出来る
#define ObjectAction(object, method, ...)	manager.GetObject(object)->##method(__VA_ARGS__)

int main()
{
	DataClass data;
	Manager manager(data);

	std::wcout << "=== Smart Pointer Test ===" << std::endl;
	auto isLoop = true;
	do
	{
		try
		{
			std::wcout << "\nPlease select number.\n1.Generator Hoge  2.Generator Piyo  3.Hoge Function  4.Piyo Function  5.Release Object  6.End" << std::endl;
			wchar_t input;
			std::wcin >> input;
			switch (input)
			{
			case '1':
				manager.GeneratorObject(ObjectType::hoge);
				std::wcout << "Success generator Hoge." << std::endl;
				break;
			case '2':
				manager.GeneratorObject(ObjectType::piyo);
				std::wcout << "Success generator Piyo." << std::endl;
				break;
			case '3':
				// マクロで簡易的に書く方法
				if (ObjectAction(manager.HogeObject, GetFlag, false))
					std::wcout << "value:" << ObjectAction(manager.HogeObject, GetValue) << std::endl;
				break;
			case '4':
				// 普通の方法
				if (const auto obj = manager.PiyoObject.lock())
					std::wcout << "Piyo Result:" << obj->TestFunc() << std::endl;
				else
					throw std::runtime_error("Non instance.");
				break;
			case '5':
				manager.ReleaseObject();
				break;
			case '6':
				isLoop = false;
				break;
			default:
				std::wcout << "Error : Invalied Input." << std::endl;
				continue;
			}
		}
		catch (const std::exception& ex)
		{
			std::wcout << "Exception:" << ex.what() << std::endl;
		}
	} while (isLoop);

	std::wcout << "finish." << std::endl;
}

// プログラムの実行: Ctrl + F5 または [デバッグ] > [デバッグなしで開始] メニュー
// プログラムのデバッグ: F5 または [デバッグ] > [デバッグの開始] メニュー

// 作業を開始するためのヒント: 
//    1. ソリューション エクスプローラー ウィンドウを使用してファイルを追加/管理します 
//   2. チーム エクスプローラー ウィンドウを使用してソース管理に接続します
//   3. 出力ウィンドウを使用して、ビルド出力とその他のメッセージを表示します
//   4. エラー一覧ウィンドウを使用してエラーを表示します
//   5. [プロジェクト] > [新しい項目の追加] と移動して新しいコード ファイルを作成するか、[プロジェクト] > [既存の項目の追加] と移動して既存のコード ファイルをプロジェクトに追加します
//   6. 後ほどこのプロジェクトを再び開く場合、[ファイル] > [開く] > [プロジェクト] と移動して .sln ファイルを選択します
