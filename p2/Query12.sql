select p.age, abs(sum(p.popestimate2011 - p.popestimate2010)), 
		case when sum(p.popestimate2011 - p.popestimate2010) > 0 then "increased"
			 when sum(p.popestimate2011 - p.popestimate2010) = 0 then "same"
			 when sum(p.popestimate2011 - p.popestimate2010) < 0 then "decreased" 
		end
from pop_estimate_state_age_sex_race_origin p
where p.sex = 0	and p.origin = 0
group by p.age;