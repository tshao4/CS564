select d.division_desc, t.stname, max(t.increase)
from division d, (select h.division, h.stname, h.state, h.huest_2011 - h.huest_2010 as increase 
					from housing_units_state_level h) t
where d.division_cd = t.division
group by t.division;