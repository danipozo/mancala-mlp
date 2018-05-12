create table state_reward (
  state                   text not null,
	                  
  occurrences             integer,
  cumulative_reward       real,
  
  primary key (state)
)
