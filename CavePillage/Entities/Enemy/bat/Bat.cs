using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Bat : Enemy
{
    PlayerAttack playerAttack;
    void Awake()
    {
        base.Awake();
        playerAttack = player.GetComponent<PlayerAttack>();
        startingHealth = 4;
        defense = 2;
        type = EnemyType.BAT;
        remainingHealth = startingHealth;
        timeBetweenAttacks = 2.5f;
        attackDamage = 4;
        chanceToDrop = 25f;
        score = 15;
        SpawnManager.ChangeNumEnemies(1);
    }

    public override void Attack ()
    {
        // Reset the timer.
        attackTimer = 0f;
        if(!playerHealth.GetIsDead())
        {
            playerHealth.TakeDamage(attackDamage);
            playerAttack.ChangeMeleeDamage(-1);
        }
    }
    // Update is called once per frame
    void Update()
    {
        CheckStatus();
        
    }
}
