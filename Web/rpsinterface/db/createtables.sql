drop table bufferclients;
create table bufferclients (sensortype char(50), client char(50), 
				protocol char(50), address char(50), port char(50));
drop table streamclients;
create table streamclients (sensortype char(50), client char(50),
                             protocol char(50), address char(50), port char(50));
quit