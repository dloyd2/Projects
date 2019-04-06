using System.Collections;
using System.Collections.Generic;
using UnityEngine;
enum DropType{HEALTH = 0, STAMINA, DEFENSE, ATTACK};
public class DropManager : MonoBehaviour
{
    // Start is called before the first frame update
    static GameObject toDrop;
    static Dictionary<DropType, GameObject>drops = new Dictionary<DropType, GameObject>();
    static float chanceToDropMany = 10f;
    static float chanceToDropHealth = 35f;
    static float chanceToDropDefense = 20f;
    static float chanceToDropAttack = 20f;
    static float chanceToDropStamina = 25f;
    static DropType toSpawn;
    static int percentResult;
    void Awake()
    {
        if(!drops.ContainsKey(DropType.HEALTH))
            drops.Add(DropType.HEALTH, Resources.Load("PickUps/HealthPickup") as GameObject);
        if(!drops.ContainsKey(DropType.STAMINA))
            drops.Add(DropType.STAMINA, Resources.Load("PickUps/StaminaPickup") as GameObject);
        if(!drops.ContainsKey(DropType.DEFENSE))
            drops.Add(DropType.DEFENSE, Resources.Load("PickUps/DefensePickup") as GameObject);
        if(!drops.ContainsKey(DropType.ATTACK))
            drops.Add(DropType.ATTACK, Resources.Load("PickUps/AttackPickup") as GameObject);
    }
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
    }
    
    static void DetermineDrop()
    {
        percentResult = Random.Range(1, 101);
        if(percentResult < chanceToDropHealth)
        {
            
            toSpawn = DropType.HEALTH;
        }
        else if(percentResult >= chanceToDropHealth && percentResult < chanceToDropHealth+chanceToDropDefense)
        {
            toSpawn = DropType.DEFENSE;
        }
        else if(percentResult >= chanceToDropHealth+chanceToDropDefense && percentResult < chanceToDropHealth+chanceToDropDefense+chanceToDropAttack)
        {
            toSpawn = DropType.ATTACK;
        }
        else if(percentResult >= chanceToDropHealth+chanceToDropDefense+chanceToDropAttack && percentResult < chanceToDropHealth+chanceToDropDefense+chanceToDropAttack+chanceToDropStamina)
        {
            toSpawn = DropType.STAMINA;
        }
    }
    static public void Drop(GameObject pos, float chanceDrop)
    {
        if(Random.Range(0, 100) <= chanceDrop)
        {
            int numToDrop = Random.Range(0, 100) < chanceToDropMany ? 2 : 1;
            Debug.Log("Dropping " + numToDrop + " objects.");
            for(int i =0; i < numToDrop; i++)
            {
                DetermineDrop();
                Debug.Log("toSpawn is " + toSpawn);
                toDrop = Instantiate(drops[toSpawn]);
                toDrop.transform.position = pos.transform.position;
                toDrop.GetComponent<Rigidbody2D>().AddForce(new Vector2((float)Random.Range(40, 100), (float)Random.Range(100, 300)));
            }
        }
    }
}
