# Historical Data
## Notes
* IBKR's historical request `m_pClient->reqHistoricalData(4001, contract, queryTime, "1 D", "5 secs", "MIDPOINT", 1, 1, false, TagValueListSPtr());`
returns the last 1 **business**/**trading** day's bars. 

## Todo
* run `process_date()` in another thread so the Arrow manipulation and I/O operations do not block the event queue