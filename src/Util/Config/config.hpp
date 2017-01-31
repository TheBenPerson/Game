#ifndef HG_Util_Config_H

class Config {

	public:

		static Config * LoadConfig(const char * Path);
		static void WriteConfig(Config * PConfig);
		
		void AddValue(const char * Name, int Value);
		int GetValue(const char * Name);
		void RemoveValue(const char * Name);
		void SetValue(const char * Name, int Value);

	private:
	
		struct Value {
		
			struct Value * NextValue;
			struct Value * PrevValue;
			
			char * Name;
			
			long Value;
			
			struct Value * GetLast();
		
		};
		
		struct Value * Values;

		Config();
		~Config();

};

#define HG_Util_Config_H
#endif
