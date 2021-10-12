import pandas as pd
df = pd.read_excel("tables.xlsx", sheet_name="globalparams")
print(df.to_latex())